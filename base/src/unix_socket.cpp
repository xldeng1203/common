#include "unix_socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <dlfcn.h>
#include <pthread.h>
#include <netdb.h>

UnixSocket::UnixSocket()
{
    m_iSocketFD = -1;
    m_iStatus = _CLOSED;

    m_iReadBegin = m_iReadEnd = 0;
    m_iSendBegin = m_iSendEnd = 0;
}

UnixSocket::~UnixSocket()
{
    if (_OPENED == m_iStatus && m_iSocketFD > 0)
    {
        Close();
    }
}

int UnixSocket::SetupFD(int iFD)
{
    if (iFD < 0)
    {
        return -1;
    }

    if (m_iSocketFD > 0 && _OPENED == m_iStatus)
    {
        Close();
    }
    
    m_iSocketFD = iFD;
    m_iStatus = _OPENED;
    m_iReadBegin = m_iReadEnd = 0;
    m_iSendBegin = m_iSendEnd = 0;

    int iFlags;
    iFlags = fcntl(m_iSocketFD, F_GETFL, 0);
    iFlags |= O_NONBLOCK;
    iFlags |= O_NDELAY;
    fcntl(m_iSocketFD, F_SETFL, iFlags);

    return 0;
}

int UnixSocket::GetOnePakcet(unsigned short &ushCodeLength, unsigned char *pCode)
{
    int iDataLength = 0;
    unsigned short ushTempLength;

    if ( !pCode )
    {
        return -1;
    }
    
    iDataLength = m_iReadEnd = m_iReadBegin;

    if ( iDataLength <= 0 )
    {
        return 0;
    }

    if ( iDataLength < (int)sizeof(short) )
    {
        if ( m_iReadEnd == sizeof(m_abyRecvBuffer) )
        {
            memcpy((void *)&m_abyRecvBuffer[0], (const void *)&m_abyRecvBuffer[m_iReadBegin], iDataLength);
            m_iReadBegin = 0;
            m_iReadEnd = iDataLength;
        }
        
        return 0;
    }
    
    memcpy((void *)&ushTempLength, (const void *)&m_abyRecvBuffer[m_iReadBegin], sizeof(short));

    if ((unsigned int)iDataLength < ushTempLength + sizeof(short))
    {
        if ( m_iReadEnd == sizeof(m_abyRecvBuffer))
        {
            memmove((void *)&m_abyRecvBuffer[0], (const void *)&m_abyRecvBuffer[m_iReadBegin], iDataLength);
            m_iReadBegin = 0;
            m_iReadEnd = iDataLength;
        }
    }

    ushCodeLength = ushTempLength;
    memcpy((void *)pCode, (const void *)&m_abyRecvBuffer[m_iReadBegin], ushCodeLength);

    m_iReadBegin += ushTempLength;
    if ( m_iReadBegin == m_iReadBegin )
    {
        m_iReadBegin = m_iReadEnd = 0;
    }
    
    return 1;
}

int UnixSocket::SendOnePacket( unsigned short ushCodeLength, unsigned char *pCode)
{
    int iBytesSend = 0;
    int iBytesLeft = ushCodeLength;
    unsigned char *pbyTemp = NULL;
    int iRet = 0;
    if (!pCode)
    {
        return ERR_SOCK_ERROR;
    }

    if ( m_iSocketFD < 0 || _OPENED != m_iStatus)
    {
        return ERR_SOCK_ERROR;
    }

    //首先检查是否有滞留数据
    iBytesLeft = m_iSendEnd - m_iSendBegin;
    pbyTemp = &(m_abyRecvBuffer[m_iSendBegin]);
    while ( iBytesLeft > 0)
    {
        iBytesSend = send(m_iSocketFD, (const char *)pbyTemp, iBytesLeft, 0);
        if (iBytesSend > 0)
        {
            pbyTemp += iBytesSend;
            iBytesLeft -= iBytesSend;
        }

        if (iBytesSend < 0 && errno != EAGAIN)
        {
            //出现发送错误，关闭流
            Close();
            iRet = ERR_SOCK_FAILED;
            break;
        } 
        else if ( iBytesSend < 0)
        {
            //发送缓冲已满，停止发送，剩余数据保持原位
            iRet = ERR_SOCK_ERROR;
            break;
        }             
    }
    
    if ( iBytesLeft == 0)
    {
        //滞留数据发送成功，则继续发送本次提交的数据
        m_iSendBegin = m_iSendEnd = 0;
    }
    else
    {
        //否则，直接返回
        return iRet;
    }

    //发送本次提交的数据
    iBytesLeft = ushCodeLength;
    pbyTemp = pCode;

    while ( iBytesLeft > 0)
    {
        iBytesSend = send(m_iSocketFD, (const char *)pbyTemp, iBytesLeft, 0);
        if (iBytesSend > 0)
        {
            pbyTemp += iBytesSend;
            iBytesLeft -= iBytesSend;
        }

        if (iBytesSend < 0 && errno != EAGAIN)
        {
            //出现发送错误，关闭流
            Close();
            iRet = ERR_SOCK_FAILED;
            break;
        }
        else if (iBytesSend < 0)
        {
            //socket发送缓冲区满，则将剩余的数据放到缓存区
            memcpy((void *)&(m_abyRecvBuffer[m_iSendEnd]), (const void *)pbyTemp, iBytesLeft);
            m_iSendEnd += iBytesLeft;
            iRet = ERR_SOCK_CLOSED;
            break;
        }
    }
 
    return iRet;    
}

int UnixSocket::Close()
{
    if (m_iSocketFD > 0)
    {
        close(m_iSocketFD);
    }

    m_iSocketFD = -1;
    m_iStatus = _CLOSED;
    
    return 0;
}

int UnixSocket::Recv()
{
    int iRecvedBytes = 0;
    int iRet = 0;
    if ( m_iSocketFD < 0 || _OPENED != m_iStatus )
    {
        return ERR_SOCK_ERROR;
    }

    if ( m_iReadEnd == m_iReadBegin)
    {
        m_iReadBegin = m_iReadEnd =  0;
    }
    
    do
    {
        //如果接收缓冲区已满，停止接收数据
        if (m_iReadEnd == sizeof(m_abyRecvBuffer))
        {
            iRet = ERR_SOCK_ERROR;
            break;
        }

        iRecvedBytes = recv(m_iSocketFD, &m_abySendBuffer[m_iReadEnd], sizeof(m_abySendBuffer) - m_iReadEnd, 0);
        if (iRecvedBytes > 0)
        {
            m_iReadEnd += iRecvedBytes;
        }
        else if ( iRecvedBytes == 0)
        {
            Close();
            iRet = ERR_SOCK_ERROR;
            break;
        }
        else if ( errno != EAGAIN)
        {
            Close();
            iRet = ERR_SOCK_FAILED;
            break;
        }
    } while ( iRecvedBytes > 0);
    
    return iRet;
}

int UnixSocket::GetIntOnePacket(int &iCodeLength, unsigned char *pCode)
{
    int iDataLength = 0;
    int iTempLength;

    if (!pCode)
    {
        return -1;
    }
    
    iDataLength = m_iReadEnd - m_iReadBegin;
    if (iDataLength <= 0)
    {
        return 0;
    }
    
    if (iDataLength < (int)sizeof(int))
    {
        if (m_iReadEnd == sizeof(m_abyRecvBuffer))
        {
            memcpy((void *)&m_abyRecvBuffer[0], (const void *)&m_abyRecvBuffer[m_iReadBegin], iDataLength);
            m_iReadBegin = 0;
            m_iReadEnd = iDataLength;
        } 
        return 0;
    }

    memcpy((void *)&iTempLength, (const void *)&m_abyRecvBuffer[m_iReadBegin], sizeof(int));

    if (iDataLength < iTempLength)
    {
        if (m_iReadEnd == sizeof(m_abyRecvBuffer))
        {
            memmove((void *)&m_abyRecvBuffer[0], (const void *)&m_abyRecvBuffer[m_iReadBegin], iCodeLength);
            m_iReadBegin = 0;
            m_iReadEnd= iDataLength;
        }
        return 0;
    }
    
    iCodeLength = iTempLength;
    memcpy((void *)pCode, (const void *)&m_abyRecvBuffer[m_iReadBegin], iCodeLength);
    
    m_iReadBegin += iTempLength;
    if (m_iReadBegin == m_iReadEnd)
    {
        m_iReadBegin = m_iReadEnd = 0;
    }
    
    return 1;
}

int UnixSocket::sendIntOnePacket(int iCodeLength, unsigned char *pCode)
{
    int iBytesSend = 0;
    int iBytesLeft = iCodeLength;
    unsigned char *pbyTemp = NULL;
    int iRet = 0;

    if ( !pCode )
    {
        return ERR_SOCK_ERROR;
    }

    if (m_iSocketFD < 0 || _OPENED != m_iStatus)
    {
        return ERR_SOCK_ERROR;
    }

    //首先检查是否有滞留数据
    iBytesLeft = m_iSendEnd - m_iSendBegin;
    pbyTemp = &(m_abySendBuffer[m_iSendBegin]);
    while ( iBytesLeft > 0 )
    {
        iBytesSend = send(m_iSocketFD, (const char *)pbyTemp, iBytesLeft, 0);
        if (iBytesSend > 0)
        {
            pbyTemp += iBytesSend;
            m_iSendBegin += iBytesSend;
            iBytesLeft -= iBytesSend;
        }

        if (iBytesSend < 0 && errno != EAGAIN)
        {
            //出现发送错误，关闭流
            Close();
            iRet = ERR_SOCK_FAILED;
            break;
        }
        else if ( iBytesSend < 0 )
        {
            //发送缓冲已满，停止发送，剩余数据保持原位
            iRet = ERR_NO_BUFFER;
            break;
        }       
    }

    if (iBytesLeft == 0)
    {
        //滞留数据发送成功，则继续发送本次提交的数据
        m_iSendBegin = m_iSendEnd = 0;
    }
    else
    {
        //否则，直接返回
        if (iCodeLength > (int)sizeof(m_abySendBuffer) - iBytesLeft) 
        {
            //此次的packet比剩余空间大
            iRet = ERR_NO_BUFFER;
        }
        else
        {
            if (iCodeLength > (int)sizeof(m_abySendBuffer) - iBytesLeft)
            {
                //此次的packet比尾部空间大
                memmove(&m_abySendBuffer[0], &m_abySendBuffer[m_iSendBegin], m_iSendEnd - m_iSendBegin);
                m_iSendEnd -= m_iSendBegin;
                m_iSendBegin = 0;
            }
            
            memcpy((void *)&(m_abySendBuffer[m_iSendEnd]), (const void *)pCode, iCodeLength);
            m_iSendEnd += iCodeLength;
            iRet = 0;
        }
        return iRet;
    }
    
    //发送本次提交的数据
    iBytesLeft = iCodeLength;
    pbyTemp = pCode;

    while (iBytesLeft > 0)
    {
        iBytesSend = send(m_iSocketFD, (const char *)pbyTemp, iBytesLeft, 0);
        if (iBytesSend > 0)
        {
            pbyTemp += iBytesSend;
            iBytesLeft -= iBytesSend;
        }
        
        if ( iBytesSend < 0 && errno != EAGAIN)
        {
            //出现发送错误，关闭流
            Close();
            iRet = ERR_SOCK_FAILED;
            break;
        }
        else if (iBytesSend < 0)
        {
            //Socket发送缓冲区满，则将剩余的数据放到缓冲中
            memcpy((void *)&(m_abySendBuffer[m_iSendEnd]), (const void *)pbyTemp, iBytesLeft);
            m_iSendEnd += iBytesLeft;
            iRet = ERR_SOCK_ERROR;
            break;
        }
    }
    
    return iRet;
}

int UnixSocket::LeftLength()
{
    return m_iSendEnd - m_iSendBegin;
}

int UnixSocket::SendLeft()
{
    int iBytesSend = 0;
    int iBytesLeft = 0;
    unsigned char *pbyTemp = NULL;
    int iRet = 0;

    if (m_iSocketFD < 0 || _OPENED != m_iStatus)
    {
        return ERR_SOCK_ERROR;
    }
     
    //首先检查是否有滞留数据
    iBytesLeft = m_iSendEnd - m_iSendBegin;
    pbyTemp = &(m_abySendBuffer[m_iSendBegin]);
    while ( iBytesLeft > 0)
    {
        iBytesLeft = send(m_iSocketFD, (const char *)pbyTemp, iBytesLeft, 0);
        if (iBytesSend > 0)
        {
            pbyTemp += iBytesSend;
            m_iSendBegin += iBytesSend;
            iBytesLeft -= iBytesSend;
        }
        
        if (iBytesSend > 0 && errno != EAGAIN)
        {
            //出现发送错误，关闭流
            Close();
            iRet = ERR_SOCK_FAILED;
            break;
        }
        else if ( iBytesSend < 0 )
        {
            //发送缓冲已满，停止发送，剩余数据保持原位
            iRet = ERR_NO_BUFFER;
            break;
        }
    }
    
    return iRet;
}