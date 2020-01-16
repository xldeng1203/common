
/*******************************************************************
** File Name:       timer.h
** Copy Right:      neil deng
** File creator:    neil deng
** Date:            
** Version:
** Desc:            
********************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_

#include "object_pool.h"
#include "utility.h"
#include "singleton.h"

#define MAX_TIMER_ATTACH_NUM 8
#define MAX_TIMER_NUM 30000

typedef struct
{
     int m_iTimeoutMS;   //定时器超时时间, ms为单位
     int m_iTimerID;     //启动定时器时没用，定时器到期的时候会设置这个id
     int m_aiAttachInfo[MAX_TIMER_ATTACH_NUM];    //定时器触发时的附加信息
} TTimerItem;

typedef int (*HandleTimeout) (TTimerItem* pstEvent);

class Timer : public Object
{
public:
     Timer() {}
     virtual ~Timer() {}

public:
     virtual int Initialize();

     inline int RegisterHandler(HandleTimeout pfcHandler) 
     {
          m_pfcHandler = pfcHandler;
          return 0;
     }

     timeval m_tvCreateTimeval;
     TTimerItem m_stTimerItem;
     HandleTimeout m_pfcHandler;
};

class TimerMgr : public Singleton< TimerMgr >
{
public:
     TimerMgr();
     ~TimerMgr();

     int Initialize(int iTimerCount = MAX_TIMER_NUM);
     int CreateTimer(TTimerItem* pTimerItem, HandleTimeout pfcHandler, timeval stCreateTimeval);
     int DestoryTimer(int iTimerID);
     Timer* GetTimerByID(int iTimerID);
     int CheckTimeout(timeval tvNow);
     int UsedCount();
     int FreeCount();

private:
     ObjectCreator* m_pTimerCreator;

public:
     static Object* funcCreateTimer(void *pMem);
};

#endif