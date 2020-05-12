#include "tcp_socket.h"

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
#include <netinet/tcp.h>


#include "log.h"

TcpListener::TcpListener()
{
    m_iSocketFD = -1;
    m_iStatus = socket_closed;
}

TcpListener::~TcpListener()
{
    if (m_iStatus == socket_opened && m_iSocketFD > 0)
    {
        Close();
    }
}

int TcpListener::CreateServer(char* szIPAddr, unsigned short unPort, int iListenCount)
{
    sockaddr_in stTempSockAddr;
    int iReusePortFlag = 1;

    if (m_iStatus != socket_closed && m_iSocketFD > 0)
    {
        Close();
    }

    m_iSocketFD = socket(AF_INET, SOCK_STREAM, 0);

    if (m_iSocketFD < 0)
    {
        m_iStatus = socket_closed;
        m_iSocketFD = -1;
        printf("socket failed \n");
    }

    if (setsockopt(m_iSocketFD, SOL_SOCKET, SO_REUSEADDR, &iReusePortFlag, sizeof(int)))
    {      
        printf("Set socket addr reusable failed, %s. \n", strerror(errno));
    }
    
    if (setsockopt(m_iSocketFD, SOL_SOCKET, SO_REUSEPORT, &iReusePortFlag, sizeof(int)))
    {      
        printf("Set socket port reusable failed, %s. \n", strerror(errno));
    }

    memset((void *)&stTempSockAddr, 0, sizeof(sockaddr_in));
    stTempSockAddr.sin_family = AF_INET;
    stTempSockAddr.sin_port = htons(unPort);

    if (szIPAddr)
    {
        stTempSockAddr.sin_addr.s_addr = inet_addr(szIPAddr);
    }
    else
    {
        stTempSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    
    if (bind(m_iSocketFD, (const struct sockaddr *)&stTempSockAddr, sizeof(stTempSockAddr)))
    {
        Close();
        printf("Bind failed, %s. \n", strerror(errno));
        return -1;
    }

    if (listen(m_iSocketFD, iListenCount) < 0)
    {
        Close();
        printf("Listen failed, %s. \n", strerror(errno));
        return -1;
    }
    
    TcpConnectionInt::SetNonBlock(m_iSocketFD);
    m_iStatus = socket_opened;

    return 0;
}

int TcpListener::Close()
{
    if (m_iSocketFD > 0)
    {
        close(m_iSocketFD);
    }
       
    m_iSocketFD = -1;
    m_iStatus = socket_closed;

    return 0;
}

/*************************   TcpConnectInt  ******************************/
TcpConnectionInt::TcpConnectionInt()
{
    m_abyRecvBuffer = NULL;
    m_abySendBuffer = NULL;
    m_iSocketFD = -1;
    m_iStatus = socket_closed;

#ifdef _MULTITHREAD_
    pThread_mutex_init( &m_stMutex, NULL);
#endif

    m_iReadBegin = m_iReadEnd = 0;
    m_iSendBegin = m_iSendEnd = 0;
}

TcpConnectionInt::~TcpConnectionInt()
{
    if (m_abyRecvBuffer)
    {
        delete []m_abyRecvBuffer;
        m_abyRecvBuffer = NULL;
    }

    if (m_abySendBuffer)
    {
        delete []m_abySendBuffer;
        m_abySendBuffer = NULL;
    }
}


int TcpConnectionInt::AllocUserBuffer(int iUsrRecvBufSize /*=0*/, int iUsrSendBufSize /*=0*/)
{
    if (m_abyRecvBuffer)
    {
        delete []m_abyRecvBuffer;
        m_abyRecvBuffer = NULL;
    }

    if (m_abySendBuffer)
    {
        delete []m_abySendBuffer;
        m_abySendBuffer = NULL;
    }

    m_iUsrRecvBufSize = iUsrRecvBufSize;
    m_iUsrSendBufSize = iUsrSendBufSize;

    if (m_iUsrRecvBufSize < 0)
    {
        m_iUsrRecvBufSize = DEFAULT_RECV_USER_BUFFER_SIZE;
    }

    if (m_iUsrSendBufSize < 0)
    {
        m_iUsrSendBufSize = 0;
    }

    m_abyRecvBuffer = new unsigned char[m_iUsrRecvBufSize];
    if (m_abyRecvBuffer == NULL)
    {
        return -2;
    }
    
    if (m_iUsrSendBufSize > 0)
    {
        m_abySendBuffer = new unsigned char[m_iUsrSendBufSize];
        if (m_abySendBuffer == NULL)
        {
            return -3;
        }
    }

    return 0;
}


int TcpConnectionInt::Recv()
{
    int iRecvedBytes = 0;

#ifdef _MULTITHREAD_
    pThread_mutex_lock( &m_stMutex);
#endif

    if (m_iSocketFD < 0 || m_iStatus != socket_connected)
    {
#ifdef _MULTITHREAD_
    pThread_mutex_unlock( &m_stMutex);
#endif   
        return recv_socket_error;
    }

    if (m_iUsrRecvBufSize <= 0 || m_abyRecvBuffer == NULL)
    {
#ifdef _MULTITHREAD_
    pThread_mutex_unlock( &m_stMutex);
#endif  
        ERROR_LOG("TcpConnectionInt::Recv() failed, m_iUsrRecvBufSize:%d \n", m_iUsrRecvBufSize);
        return recv_socket_error;
    }
    
    //缓冲区已经清空，复位
    if (m_iReadEnd == m_iReadBegin)
    {
        m_iReadBegin = 0;
        m_iReadEnd = 0;
    }
    
    do
    {
        if ( m_iReadEnd == m_iUsrRecvBufSize)
        {
            /* 如果指示数据开始的指针(m_iReadBegin)不在缓冲区的起点，则通过移动缓冲区中的数据，还可以继续接收数据 */
            if (m_iReadBegin > 0)
            {
                memmove(&m_abyRecvBuffer[0], &m_abyRecvBuffer[m_iReadBegin], m_iReadEnd - m_iReadBegin);
                m_iReadEnd -= m_iReadBegin;
                m_iReadBegin = 0;
            }
            //用户的接收缓冲区已满
            else
            {                
#ifdef _MULTITHREAD_
                pThread_mutex_unlock( &m_stMutex);
#endif               
                return recv_socket_error;
            }
        }
        
        iRecvedBytes = recv(m_iSocketFD, &m_abyRecvBuffer[m_iReadEnd],  m_iUsrRecvBufSize-m_iReadEnd, 0);
        if (iRecvedBytes > 0)
        {
            m_iReadEnd += iRecvedBytes;
        }
        //远程关闭链接
        else if ( iRecvedBytes == 0)
        {      
            Close();
#ifdef _MULTITHREAD_
                pThread_mutex_unlock( &m_stMutex);
#endif 
            return recv_socket_error;
        }
        //接收发生错误
        else if (errno != EAGAIN)
        {
            Close();
#ifdef _MULTITHREAD_
            pThread_mutex_unlock( &m_stMutex);
#endif 
            return recv_failed;
        }     
    } while (iRecvedBytes > 0);

#ifdef _MULTITHREAD_
            pThread_mutex_unlock( &m_stMutex);
#endif   
    
    return 0;
}

int TcpConnectionInt::Close()
{
    if (m_iSocketFD > 0)
    {
        close(m_iSocketFD);
    }

    m_iSocketFD = -1;
    m_iStatus = socket_closed;

    return 0;
}

/*
    消息结构：
    sizeof(unsigned short)   + sizeof(unsigned int) + buffer
    包开始表示                +  整个包的长度
    unsigned int *uiPacketLength = buffer 长度
*/
int TcpConnectionInt::GetOnePacket(int &iPacketLength, unsigned char* pPacketBuff)
{
    iPacketLength = 0;

    if (!pPacketBuff)
    {
        return -1;
    }

    int iDataLength = m_iReadEnd - m_iReadBegin;
    if (iDataLength < 0)
    {
        return 0;
    }
    
    enum
    {
        msglen = sizeof(unsigned int),
    };

    if (iDataLength < msglen)   
    {
        //如果RecvBuffer里的数据比开头表示+msglen还小
        if (m_iReadEnd == m_iUsrRecvBufSize)
        {
            //从后边移动到开头
            memmove((void *)&m_abyRecvBuffer[0], (const void *)&m_abyRecvBuffer[m_iReadBegin], iDataLength);
            m_iReadBegin = 0;
            m_iReadEnd = iDataLength;
        }
        return 0;
    }

    iPacketLength = *(int *)(&m_abyRecvBuffer[m_iReadEnd]);
    if (iPacketLength > MAX_TCPPACKETINT_SIZE || iPacketLength <= 0)
    {
        return -3;
    }
    
    //长度还不够这个packet
    if (iDataLength < msglen + iPacketLength)
    {   
        if (m_iReadEnd == m_iUsrRecvBufSize)
        {
            memmove((void *)&m_abyRecvBuffer[0], (const void *)&m_abyRecvBuffer[m_iReadBegin], iDataLength);
            m_iReadBegin = 0;
            m_iReadEnd = iDataLength;
        }
        
        return 0;
    }
    
    memcpy((void *)pPacketBuff, (const void *)(&m_abyRecvBuffer[m_iReadBegin] + msglen), iPacketLength);
    m_iReadBegin += ((iPacketLength) + msglen);
    if (m_iReadBegin == m_iReadEnd)
    {
        m_iReadBegin = m_iReadEnd = 0;
    }
    
    return 1;
}

int TcpConnectionInt::SendOnePacket(int iPacketLength, unsigned char *pPacket)
{
    int iBytesSend = 0;
    unsigned char *pbyTemp = NULL;
    int iTempRet = 0;

    if (!pPacket)
    {
        return send_socket_error;
    }

#ifdef _MULTITHREAD_
    pThread_mutex_lock( &m_stMutex);
#endif

    if (m_iSocketFD < 0 || m_iStatus != socket_connected)
    {
#ifdef _MULTITHREAD_
    pThread_mutex_unlock( &m_stMutex);
#endif   
        return send_socket_error;
    }
    
    int iBytesLeft = 0;

    //如果有发送缓冲区，则检查是否有滞留数据
    if ((m_abySendBuffer != NULL) && (m_iUsrSendBufSize > 0))
    {
        iBytesLeft = m_iSendEnd - m_iSendBegin;
        pbyTemp = &(m_abySendBuffer[m_iSendBegin]);
        while ( iBytesLeft > 0)
        {
            iBytesSend = send(m_iSocketFD, (const char*)pbyTemp, iBytesLeft, 0);
            if (iBytesSend > 0)
            {
                m_iSendBegin += iBytesSend;
                pbyTemp += iBytesSend;
                iBytesLeft -= iBytesSend;
            }

            if (iBytesSend < 0 && errno != EAGAIN)
            {
                m_iStatus = socket_error;
                iTempRet = send_failed;
                break;
            }
            else if (iBytesSend < 0)
            {
                iTempRet = send_no_buffer;
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
            //否则直接返回
#ifdef _MULTITHREAD_
            pThread_mutex_unlock( &m_stMutex);
#endif 
            if (iPacketLength > m_iUsrSendBufSize - iBytesLeft)
            {
                 //此次的packet比剩余空间大
                iTempRet = send_no_buffer;
            }
            else
            {
                if (iPacketLength > m_iUsrSendBufSize - m_iSendEnd)
                {
                    //此次的packet比尾部空间大
                    memmove(&m_abySendBuffer[0], &m_abySendBuffer[m_iSendBegin], m_iSendEnd - m_iSendBegin);
                    m_iSendEnd -= m_iSendBegin;
                    m_iSendBegin = 0;
                }
                
                memcpy((void *)&(m_abySendBuffer[m_iSendEnd]), (const void *)pPacket, iPacketLength);
                m_iSendEnd += iPacketLength;
                iTempRet = 0;
            }
            
            return iTempRet;
        }
    }
    
    //发送本次提交的数据
    iBytesLeft = iPacketLength;
    pbyTemp = pPacket;

    while ( iBytesLeft > 0)
    {
        iBytesSend = send(m_iSocketFD, (const char*)pbyTemp, iBytesLeft, 0);
        if (iBytesSend > 0)
        {
            pbyTemp += iBytesSend;
            iBytesLeft -= iBytesSend;
        }

        if (iBytesSend < 0 && errno != EAGAIN)
        {
            m_iStatus = socket_error;
            iTempRet = send_failed;
            break;
        }
        else if ( iBytesSend < 0)
        {
            //socket发送缓冲区满，则将生于的数据发到缓冲中
            if ((m_abySendBuffer != NULL) && (m_iUsrSendBufSize > 0))
            {
                memcpy((void *)&(m_abySendBuffer[m_iSendEnd]), (const void *)pbyTemp, iBytesLeft);
                m_iSendEnd += iBytesLeft;
                iTempRet = 0;
            }
            else
            {
                iTempRet = send_no_buffer;
            }
            break;
        }           
    }  
#ifdef _MULTITHREAD_
            pThread_mutex_unlock( &m_stMutex);
#endif

    return iTempRet;
}


int TcpConnectionInt::SetSysBuffLen(int iFD, int iSysRecvBufLen, int iSysSendBufLen)
{
    int iOptLen = sizeof(int);

    if (iSysRecvBufLen > 0)
    {
        int iRet = setsockopt(iFD, SOL_SOCKET, SO_RCVBUF, &iSysRecvBufLen, iOptLen);
        if (iRet)
        {
            return -1;
        }
    }

    int iRet = getsockopt(iFD, SOL_SOCKET, SO_RCVBUF, (void *)&iSysRecvBufLen, (socklen_t *)&iOptLen);
    if (iRet)
    {
        return -2;
    }

    if (iSysSendBufLen > 0)
    {
        int iRet = setsockopt(iFD, SOL_SOCKET, SO_SNDBUF, &iSysSendBufLen, iOptLen);
        if (iRet)
        {
            return -3;
        }
    }

    iRet = getsockopt(iFD, SOL_SOCKET, SO_SNDBUF, (void *)&iSysSendBufLen, (socklen_t *)&iOptLen);
    if (iRet)
    {
        return -4;
    }
    
    return 0;
}

int TcpConnectionInt::SendLeft()
{
    if ((m_abySendBuffer == NULL ) || (m_iUsrSendBufSize == 0))
    {
        return 0;
    }

    int iBytesSend = 0;
    int iBytesLeft = 0;
    unsigned char *pbyTemp = NULL;
    int iTempRet = 0;

#ifdef _MULTITHREAD_
    pThread_mutex_lock( &m_stMutex);
#endif 

    if (m_iSocketFD < 0 || m_iStatus != socket_connected)
    {
#ifdef _MULTITHREAD_
        pThread_mutex_unlock( &m_stMutex);
#endif 
        return send_socket_error;
    }

    //首先检查是否有滞留数据
    iBytesLeft = m_iSendEnd - m_iSendBegin;
    pbyTemp = &(m_abySendBuffer[m_iSendBegin]);

    while (iBytesLeft > 0)
    {
        iBytesSend = send(m_iSocketFD, (const char *)pbyTemp, iBytesLeft, 0);
        if (iBytesSend > 0)
        {
            m_iSendBegin += iBytesSend;
            pbyTemp += iBytesSend;
            iBytesLeft -= iBytesSend;
        }
        
        if (iBytesSend < 0 && errno != EAGAIN)
        {
            m_iStatus = socket_error;
            iTempRet = send_failed;
            break;
        }
        else if (iBytesSend < 0)
        {
            iTempRet = send_no_buffer;
            break;
        }
    }
    
    return iTempRet;
}

int TcpConnectionInt::SetupAsSvrConnection(int iNewFD)
{
    int iTempRet = 0;
    if (iNewFD < 0 )
    {
        return -1;
    }

#ifdef _MULTITHREAD_
    pThread_mutex_lock( &m_stMutex);
#endif 

    if (m_iSocketFD > 0 && m_iStatus == socket_connected)
    {
        ERROR_LOG("Warning another connection request from remote, close the previous(%d).\n", m_iSocketFD);
        Close();
    }
    
    m_iSocketFD = iNewFD;
    m_iStatus = socket_connected;
    m_iReadBegin = m_iReadEnd = 0;
    m_iSendBegin = m_iSendEnd = 0;

    TcpConnectionInt::SetNonBlock(m_iSocketFD);

#ifdef _MULTITHREAD_
    pThread_mutex_unlock( &m_stMutex);
#endif 

    return iTempRet;
}

int TcpConnectionInt::SetupAsClient(char * szLocalAddr)
{
    if (m_iStatus != socket_closed && m_iSocketFD > 0)
    {
        Close();
    }
    
    m_iSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (m_iSocketFD < 0)
    {
        m_iStatus = socket_closed;
        m_iSocketFD = -1;
        return -1;
    }
    
    if (szLocalAddr)
    {
        sockaddr_in stTempSockAddr;
        memset((void *)&stTempSockAddr, 0 , sizeof(sockaddr_in));
        stTempSockAddr.sin_family = AF_INET;
        stTempSockAddr.sin_addr.s_addr = inet_addr(szLocalAddr);
        bind(m_iSocketFD, (const struct sockaddr *)&stTempSockAddr, sizeof(stTempSockAddr));
    }

    m_iStatus = socket_opened;
    m_iReadBegin = m_iReadEnd = 0;
    m_iSendBegin = m_iSendEnd = 0;
    
    return 0;
}

int TcpConnectionInt::ConnectTo(char *szIPAddr, unsigned short unPort)
{
    sockaddr_in stTempAddr;
    if (!szIPAddr)
    {
        return -1;
    }

    if (m_iStatus != socket_opened || m_iSocketFD < 0)
    {
        return -2;
    }
    
    memset((void *)&stTempAddr, 0, sizeof(sockaddr_in));
    stTempAddr.sin_family = AF_INET;
    stTempAddr.sin_port = htons(unPort);
    stTempAddr.sin_addr.s_addr = inet_addr(szIPAddr);

    if (connect(m_iSocketFD, (const struct sockaddr *)&stTempAddr, sizeof(stTempAddr)))
    {
        Close();
        return -3;
    }
    
    TcpConnectionInt::SetNonBlock(m_iSocketFD);
    m_iReadBegin = m_iReadEnd = 0;
    m_iSendBegin = m_iSendEnd = 0;
    m_iStatus = socket_connected;

    return 0;
}

int TcpConnectionInt::ConnectToAsClient(char *szLocalAddr, char *szIPAddr, unsigned short unPort)
{
    if (!szIPAddr)
    {
        return -1;
    }
    
    if (m_iStatus != socket_closed && m_iSocketFD > 0)
    {
        Close();
    }
    
    char szPort[16];
    snprintf(szPort, 16, "%d", unPort);
    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype =SOCK_STREAM;

    int rv = 0;
    if ((rv = getaddrinfo(szIPAddr, szPort, &hints, &servinfo)) != 0)
    {
        return -2;
    }

    if (!servinfo)
    {
        return -3;
    }

    struct addrinfo *p;
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((m_iSocketFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            continue;
        }
        
        if (szLocalAddr)
        {
            sockaddr_in stTempSockAddr;
            memset((void *)&stTempSockAddr, 0, sizeof(sockaddr_in));
            stTempSockAddr.sin_family = AF_INET;
            stTempSockAddr.sin_addr.s_addr = inet_addr(szLocalAddr);
            bind(m_iSocketFD, (const struct sockaddr *)&stTempSockAddr, sizeof(stTempSockAddr));
        }
        
        if (connect(m_iSocketFD, p->ai_addr, (int)p->ai_addrlen))
        {
            Close();
            continue;
        }
        
        break;
    }
    
    freeaddrinfo(servinfo);

    if (!p)
    {
        return -4;
    }

    TcpConnectionInt::SetNonBlock(m_iSocketFD);

    m_iReadBegin = m_iReadEnd = 0;
    m_iSendBegin = m_iSendEnd = 0;
    m_iStatus = socket_connected;

    return 0;   
}

int TcpConnectionInt::SetNonBlock(int iFD)
{
    int iFlags;
    iFlags = fcntl(iFD, F_GETFL, 0);
    iFlags |= O_NONBLOCK;
    iFlags |= O_NDELAY;
    fcntl(iFD, F_SETFL, iFlags);
    JHTcpConnectionInt::SetNoDelay(iFD);
    return 0;
}

int JHTcpConnectionInt::SetNoDelay( int iFD )
{
	int iYes = 1;
	if (setsockopt(iFD, IPPROTO_TCP, TCP_NODELAY, &iYes, sizeof(iYes)) == -1)
	{
		printf("SetNoDelay failed， %s.\n", strerror(errno));
		return -1;
	}
	return 0;
}

int TcpConnectionInt::LeftLength()
{
    return m_iSendEnd - m_iSendBegin;
}
