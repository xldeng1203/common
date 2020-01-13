
/*
     File creator: neil deng
*/

#ifndef __EPOLLHELPER_H__
#define __EPOLLHELPER_H__

#include <sys/epoll.h>
#include <assert.h>

typedef int (*funcEpollHandler) (int);

class CEpollHelper
{
public:
    void SetErrorHandler(funcEpollhandler pfError);
    void SetReadHandler(funcEpollhandler pfRead);
    void SetWriteHandler(funcEpollhandler pfWrite);
public:
    int EpollCreate(int iFDSize);
    int EpollWait();
    int EpollAdd(int iFd);
    int EpollDelete(int iFd);
    int EpollFocusOut(int iFd, bool bFocus = true);
    int SetNonBlock(int iFd);

    inline void SetEpollWaitingTime(int iEpollWaitingTime);

    int GetEventErr()
    {
        return m_iEpollEventError;
    }

private:
    funcEpollHandler m_pfError;
    funcEpollHandler m_pfRead;
    funcEpollHandler m_pfWrite;

private:
    int m_iEpollFd;
    int m_iEpollEventSize;
    int m_iEpollWaitingTime;
    epoll_event* m_pstEpollEvent;
    epoll_event m_stOneEpollEvent;
    int m_iEpollEventError;

};

inline void CEpollHelper::SetEpollWaitingTime(int iEpollWaitingTime)
{
    m_iEpollWaitingTime = iEpollWaitingTime;
}

#endif