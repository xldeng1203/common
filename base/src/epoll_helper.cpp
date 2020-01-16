#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <error.h>

#include "epoll_helper.h"
#include "log.h"

void CEpollHelper::SetErrorHandler(funcEpollHandler pfError)
{
    m_pfError = m_pfError;
}

void CEpollHelper::SetReadHandler(funcEpollHandler pfRead)
{
    m_pfRead = m_pfRead;
}

void CEpollHelper::SetWriteHandler(funcEpollHandler pfWrite)
{
    m_pfWrite = pfWrite;
}

int CEpollHelper::EpollCreate(int iFDSize)
{
    m_iEpollEventSize = iFDSize;
    m_iEpollFd = epoll_create(m_iEpollEventSize);
    if(m_iEpollFd < 0 )
    {
        return -1;
    }

    memset(&m_stOneEpollEvent, 0, sizeof(m_stOneEpollEvent));
    m_pstEpollEvent = new epoll_event[m_iEpollEventSize];
    if( NULL == m_pstEpollEvent)
    {
        return -2;
    }

    //如果关注out事件， 在LT的触发模式下，每次wait都会报告事件，起不到sleep的作用，如果关注out,就用EPOLLLET模式
    //但是EPOLLET要注意：数据只有在变化的时候通知，因此要求在Recv的时候，受光系统缓冲区李的数据，也就要求UserBuf要大雨 SysBuf
    //m_stOneEpollEvent.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLET;

    //下边这种模式，不关注Out事件，也就不能SendLeft， 也就要求UserBuf尽可能小，SysBuf尽可能大
    m_stOneEpollEvent.events = EPOLLIN | EPOLLERR | EPOLLHUP;
    m_stOneEpollEvent.data.ptr = NULL;
    m_stOneEpollEvent.data.fd = -1;

    m_iEpollEventError = 0;
    return 0;
}

int CEpollHelper::EpollWait()
{
    int iEpollEventNumber = 0;
    iEpollEventNumber = epoll_wait(m_iEpollFd, m_pstEpollEvent, m_iEpollEventSize, m_iEpollWaitingTime);
    if(iEpollEventNumber < 0 )
    {
        //DEBUG_LOG("CEpollHelper::EpollWait (%d)  failed %d errno:%d, errstr:%s\n ", m_iEpollFd, i_EpollEventNumber, errno, strerror(errno));
        return -1;
    }

    for( int i = 0; i < iEpollEventNumber; ++i)
    {
        int iFd = m_pstEpollEvent[i].data.fd;
        unsigned int uiEpollEvent = m_pstEpollEvent[i].events;
        if((EPOLLERR | EPOLLHUP) & uiEpollEvent)
        {
            m_iEpollEventError = uiEpollEvent;
            (*m_pfError)(iFd);
            m_iEpollEventError = 0;
        }
        else if ((EPOLLIN) & uiEpollEvent)
        {
            (*m_pfRead)(iFd);
        }
        else if ((EPOLLOUT) & uiEpollEvent)
        {
            (*m_pfWrite)(iFd);
        }
    }

    return 0;
}

int CEpollHelper::EpollAdd(int iFd)
{
    int iRet = 0;
    m_stOneEpollEvent.data.fd = iFd;
    iRet = epoll_ctl(m_iEpollFd, EPOLL_CTL_ADD, iFd, &m_stOneEpollEvent);
    if (iRet < 0)
    {
        return -1;
    }
    
    return 0;
}

int CEpollHelper::EpollDelete(int iFd)
{
    int iRet = 0;
    m_stOneEpollEvent.data.fd = iFd;
    iRet = epoll_ctl(m_iEpollFd, EPOLL_CTL_DEL, iFd, &m_stOneEpollEvent);
    if (iRet < 0)
    {
        return -1;
    }
    
    return 0;
}

int CEpollHelper::EpollFocusOut(int iFd, bool bFocus /*=true*/)
{
    int iRet = 0;

    m_stOneEpollEvent.data.fd = iFd;
    m_stOneEpollEvent.events = EPOLLIN | EPOLLERR | EPOLLHUP;
    if (bFocus)
    {
        m_stOneEpollEvent.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLOUT;
    }

    iRet = epoll_ctl(m_iEpollFd, EPOLL_CTL_MOD, iFd, &m_stOneEpollEvent);
    if (iRet < 0)
    {
        return -1;
    }
    
    return 0;
}

int CEpollHelper::SetNonBlock(int iFd)
{
    int iFlags;
    iFlags = fcntl(iFd, F_GETFL, 0);
    iFlags |= O_NONBLOCK;
    fcntl(iFd, F_SETFL, iFlags);

    return 0;
}