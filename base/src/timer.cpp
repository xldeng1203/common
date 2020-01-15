#include "timer.h"
#include <stdio.h>
#include <stdlib.h>

int Timer::Initialize()
{
    memset(&m_stTimerItem, 0, sizeof(m_stTimerItem));
    m_pfcHandler = NULL;
    return 0;
}

/****************  TimerMgr *********************/
Object* TimerMgr::funcCreateTimer(void *pMem)
{
    Object::pObjMem = (unsigned char*)pMem;
    return (Object*)(new Timer);
}

TimerMgr::TimerMgr()
{
    m_pTimerCreator = NULL;
}

TimerMgr::~TimerMgr()
{
    if (m_pTimerCreator)
    {
        delete m_pTimerCreator;
    }
}

int TimerMgr::Initialize( int iTimerCount /* = MAX_TIMER_NUM */)
{
    m_pTimerCreator = new ObjectCreator(sizeof(Timer), iTimerCount, TimerMgr::funcCreateTimer);
    if (!m_pTimerCreator)
    {
        return -1;
    }

    return 0;
}

//返回定时器的ObjectID, 如果小于0, 则表示失败
int TimerMgr::CreateTimer(TTimerItem* pTimerItem, HandleTimeout pfcHandler, timeval stCreateTimeval)
{
    if (!pTimerItem || !pfcHandle)
    {
        return -1;
    }

    if (!m_pTimerCreator)
    {
        return -100;
    }
    
    int iObjId = m_pTimerCreator->CreateObject();
    if (iObjId < 0)
    {
        return -2;
    }
    
    Timer *pTimer = (Timer*)m_pTimerCreator->GetObject(iObjId);
    if (!pTimer)
    {
        return -3;
    }

    pTimer->Initialize(); 
    pTimerItem->m_iTimerID = iObjId;
    pTimer->RegisterHandler(pfcHandler);
    memcpy((void*)&(pTimer->m_stTimerItem), (void*)pTimerItem, sizeof(TTimerItem));
    pTimer->m_tvCreateTimeval = stCreateTimeval;

    return iObjId;
}

int TimerMgr::DestoryTimer(int iTimerID)
{
    if (!m_pTimerCreator)
    {
        return -100;
    }
    
    int iRet = m_pTimerCreator->DestoryObject(iTimerID);
    return iRet;
}

int TimerMgr::CheckTimeout( timeval tvNow)
{
    if (!m_pTimerCreator)
    {
        return -100;
    }
    
    int iTimerID = m_pTimerCreator->BusyHead();

    ObjectWrapper *pWrapper = NULL;
    Timer *pTimer = NULL;
    int iTimeoutCount = 0;

    while (iTimerID >= 0)
    {
        pWrapper = m_pTimerCreator->GetWrapper(iTimerID);
        if (!pWrapper)
        {
            return -1;
        }
        
        pTimer = (Timer*)pWrapper->GetWrapperObj();
        if (!pTimer)
        {
            return -2;
        }
        
        timeval tvPassed;
        TimevalMinutes(tvNow, pTimer->m_tvCreateTimeval, tvPassed);

        //超时了，要按下边的顺序执行
        if ((tvPassed.ev_sec * 1000 + tv_Passed.tv_usec / 1000) >= pTimer->m_stTimerItem.m_iTimeoutMS)
        {
            //1. 记下来超时的对象ID
            int iDestoryID = iTimerID;

            //2. 触发逻辑
            if (pTimer->m_pfcHandler)
            {
                pTimer->m_pfcHandler(&pTimer->m_stTimerItem);
            }
    
            //3. 获取下一个对象
            iTimerID = pWrapper->GetWrapperObj();

            //4. 销毁
            m_pTimerCreator->DestoryObject(iDestoryID);
            ++iTimeoutCount;
            continue;
        }
        
        iTimerID = pWrapper->GetNextIdx();
    }
    
    return iTimeoutCount;
}

int TimerMgr::UsedCount()
{
    if (m_pTimerCreator)
    {
        return m_pTimerCreator->BusyCount();
    }
    
    return -1;
}

int TimerMgr::FreeCount()
{
    if (m_pTimerCreator)
    {
        return m_pTimerCreator->FreeCount();
    }

    return -1;
}

Timer* TimerMgr::GetTimerByID( int iTimerID )
{
    if (!m_pTimerCreator)
    {
        return NULL;
    }
    
    Object* pTimer = m_pTimerCreator->GetObject(iTimerID);
    if (!pTimer)
    {
        return NULL;
    }
    
    return (Timer*)pTimer;
}