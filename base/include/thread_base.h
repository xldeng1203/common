/*******************************************************************
** File Name:       thread_base.h
** Copy Right:      neil deng
** File creator:    neil deng
** Date:            
** Version:
** Desc:            
********************************************************************/

#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>

#ifdef _DEBUG
#define TRACE  LogThread
#else
#define TRACE  xprintf
#endif

enum
{
     thread_init = 0,
     thread_blocked = 1,
     thread_running = 2,
     thread_stopped = 3
};

typedef struct 
{
     char sLogFile[200];
     long lMaxLogSize;
     int iMaxLogNum;
     int iInited;
} TLogCfg;

void * ThreadProc(void *pvArgs);

class ThreadBase
{
public:
     ThreadBase();
     virtual ~ThreadBase();
     virtual int Prepare() = 0;
     virtual int Run() = 0;

     int CreateThread();
     int WakeUp();
     int Stop();
     void InitLogParam(char *sPLogBaseName, long lPMaxLogSize, int iPMaxLogNum);

protected:
     int CheckCondition();
     void LogThread(const char *sFormat, ...);
     void DebugLogThread(const char *sFormat, ...);

     int  m_iCondition;
     char m_RetBuf[64];

     pthread_t           m_pthread;
     pthread_attr_t      m_stAttr;
     pthread_mutex_t     m_stMutex;

     TLogCfg             m_stLogCfg;
};




#endif