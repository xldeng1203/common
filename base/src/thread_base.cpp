#include "thread_base.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "log.h"

void* ThreadProc(void *pvArgs)
{
    if (!pvArgs)
    {
        return NULL;
    }

    ThreadBase *pThread = (ThreadBase *)pvArgs;
    if (pThread || pThread->Prepare())
    {
        return NULL;
    }

    pThread->Run();

    return NULL;
}


ThreadBase::ThreadBase()
{
    m_iCondition = thread_init;
    memset((void *)&m_stLogCfg, 0, sizeof(m_stLogCfg));
}

ThreadBase::~ThreadBase()
{

}

int ThreadBase::CreateThread()
{
    pthread_attr_init(&m_stAttr);
    pthread_attr_setscope(&m_stAttr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&m_stAttr, PTHREAD_CREATE_JOINABLE);
    pthread_mutex_init(&m_stMutex, NULL);
    m_iCondition = thread_running;
    pthread_create(&m_pthread, &m_stAttr, ThreadProc, (void *)this);

    return 0;
}

int ThreadBase::CheckCondition()
{
    pthread_mutex_lock(&m_stMutex);
    if ( m_iCondition == thread_stopped)
    {
        LogThread("Thread is dying. \n");
        pthread_exit((void *)m_RetBuf);
    }

    m_iCondition = thread_running;
    pthread_mutex_unlock(&m_stMutex);

    return 0;
}

int ThreadBase::WakeUp()
{
    return 0;
}

int ThreadBase::Stop()
{
    pthread_mutex_lock(&m_stMutex);
    m_iCondition = thread_stopped;
    pthread_mutex_unlock(&m_stMutex);

    //等待该线程终止
    pthread_join(m_pthread, NULL);
    LogThread("Thread is Died.\n");

    return 0;
}

void ThreadBase::InitLogParam(char *sPLogBaseName, long lPMaxLogSize, int iPMaxLogNum)
{
    memset(m_stLogCfg.sLogFile, 0, sizeof(m_stLogCfg.sLogFile));
    strncpy(m_stLogCfg.sLogFile, sPLogBaseName, sizeof(m_stLogCfg.sLogFile)-1);
    m_stLogCfg.lMaxLogSize = lPMaxLogSize;
    m_stLogCfg.iMaxLogNum = iPMaxLogNum;
    m_stLogCfg.iInited = 1;
}

void ThreadBase::LogThread(const char *sFormat, ...)
{
    va_list ap;

    if (m_stLogCfg.iInited != 0)
    {
        va_start(ap, sFormat);
        VWriteLog(m_stLogCfg.sLogFile, m_stLogCfg.lMaxLogSize, 
                    m_stLogCfg.iMaxLogNum, NULL, sFormat, ap);
        va_end(ap);
    }
}

void ThreadBase::DebugLogThread(const char *sFormat, ...)
{
#ifndef _DEBUG
    return;
#endif
    va_list ap;
    if (m_stLogCfg.iInited != 0)
    {
        VWriteLog(m_stLogCfg.sLogFile, m_stLogCfg.lMaxLogSize, 
                    m_stLogCfg.iMaxLogNum, NULL, sFormat, ap);
        va_end(ap);
    }
        
}