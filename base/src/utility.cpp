#ifndef _UNILITY_CPP_
#define _UNILITY_CPP_

#include "utility.h"

/************ 时间类相关 ************/
int GetTimeStruct(time_t tTime, TimeStruct *pstTime)
{
    struct tm *pTempTm = NULL;
    struct tm stTempTm;
    time_t tTemp = tTime;

    if (!pstTime)
    {
        return -1;
    }

    pTempTm = localtime_r(&tTemp, &stTempTm);
    if (!pTempTm)
    {
        return -1;
    }
    pstTime->iYear = stTempTm.tm_year + 1900;
    pstTime->iMon = stTempTm.tm_mon + 1;
    pstTime->iDay = stTempTm.tm_mday;
    pstTime->iHour = stTempTm.tm_hour;
    pstTime->iMinute = stTempTm.tm_min;
    pstTime->iSecond = stTempTm.tm_sec;

    return 0;
}

//!tvResult = tvA + tvB
int TimevalPlus(timeval& tvA, timeval& tvB, timeval &tvResult)
{
    timeval tvTemp;
    tvTemp.tv_sec = tvA.tv_sec + tvB.tv_sec;
    tvTemp.tv_sec += ((tvA.tv_usec + tvB.tv_usec) / 1000000);
    tvTemp.tv_usec = ((tvA.tv_usec + tvB.tv_usec) % 1000000);

    tvResult.tv_sec = tvTemp.tv_sec;
    tvResult.tv_usec = tvTemp.tv_usec;
    return 0;
}

//!tvResult = tvA - tvB
int TimevalMinus(timeval& tvA, timeval& tvB, timeval &tvResult)
{
    timeval tvTemp;

    if (tvA.tv_usec < tvB.tv_usec)
    {
        tvTemp.tv_usec = (1000000 + tvA.tv_usec) - tvB.tv_usec;
        tvTemp.tv_sec = tvA.tv_sec - tvB.tv_sec - 1;
    }
    else
    {
        tvTemp.tv_usec = tvA.tv_usec - tvB.tv_usec;
        tvTemp.tv_sec = tvA.tv_sec - tvB.tv_sec;
    }
    
    tvResult.tv_sec = tvTemp.tv_sec;
    tvResult.tv_usec = tvTemp.tv_usec;
    return 0;
}

bool IsSameHour(time_t tTime1, time_t tTime2)
{
    TimeStruct stTime1;
    TimeStruct stTime2;
    GetTimeStruct(tTime1, &stTime1);
    GetTimeStruct(tTime2, &stTime2);

    if (stTime1.iYear != stTime2.iYear)
    {
       return false;
    }

    if (stTime1.iMon != stTime2.iMon)
    {
       return false;
    }  

    if (stTime1.iDay != stTime2.iDay)
    {
       return false;
    } 

    if (stTime1.iHour != stTime2.iHour)
    {
       return false;
    } 

    return true;
}


bool IsSameDay(time_t tTime1, time_t tTime2)
{
    TimeStruct stTime1;
    TimeStruct stTime2;
    GetTimeStruct(tTime1, &stTime1);
    GetTimeStruct(tTime2, &stTime2);

    if (stTime1.iYear != stTime2.iYear)
    {
       return false;
    }

    if (stTime1.iMon != stTime2.iMon)
    {
       return false;
    }  

    if (stTime1.iDay != stTime2.iDay)
    {
       return false;
    } 

    return true;
}

char *GetDataTime(time_t *mytime)
{
    static char s[50];
    struct tm curr;
    curr = *localtime(mytime);
    if(curr.tm_year > 50)
    {
        sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d",
        curr.tm_year+1900, curr.tm_mon+1, curr.tm_mday, 
        curr.tm_hour, curr.tm_min, curr.tm_sec);
    }
    else
    {
        sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d",
        curr.tm_year+2000, curr.tm_mon+1, curr.tm_mday, 
        curr.tm_hour, curr.tm_min, curr.tm_sec);
    }
    
    return s;
}

int GetCurrentDay(char *strTime)
{
    struct tm *pTempTm = NULL;
    time_t timer;

    if ( !strTime)
    {
        return -1;
    }

    time( &timer );
    strTime[0] = '\0';
    pTempTm = localtime( &timer );
    if ( !pTempTm)
    {
        return -1;
    }

    sprintf(strTime, "%04d-%02d-%02d", pTempTm->tm_year + 1900, pTempTm->tm_mon + 1, pTempTm->tm_mday);
    
    return 0;
}

char *GetCurrentDataTime(void)
{
    time_t iCurrTime;
    time( &iCurrTime );
    return GetDataTime(&iCurrTime);
}

/************ 建立目录 ************/
int MakeDir(const char *pcDirPath, pthread_mutex_t *pMutex /*= NULL*/)
{
    if (pMutex)
    {
        pthread_mutex_lock( pMutex );
    }
    
    DIR* dpBill = opendir( pcDirPath );
    if ( !dpBill )
    {
#ifndef _DEBUG
        if( mkdir( pcDirPath, S_IRWXU | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH))
#else
        if( mkdir( pcDirPath, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))
#endif
        {
            if (pMutex)
            {
                pthread_mutex_unlock( pMutex );
            }
            return -1;
        }
    }
    else
    {
        closedir( dpBill );
    }

    if (pMutex)
    {
        pthread_mutex_unlock( pMutex );
    }

    return 0;
}

/************ 字符串相关 ************/
void TrimStr( char *strInput)
{  
    if ( strInput == NULL )
    {
        return;
    }

    char *pb = NULL;
    char *pe = NULL;
    int iTempLen = 0;
    iTempLen = strlen(strInput);
    if (iTempLen == 0)
    {
        return;
    }
    
    pb = strInput;
    while(((*pb == ' ') || (*pb == '\t') || (*pb == '\n') || (*pb == '\r')) && (*pb != 0))
    {
        pb ++;
    }

    pe = &strInput[iTempLen-1];
    while((pe >= pb) && ((*pe == ' ') || (*pe == '\t') || (*pe == '\n') || (*pe == '\r')))
    {
        pe --;
    }

    *(pe+1) = '\0';
    
    int iLen = strlen(pb) + 1;
    memcpy(strInput, pb, iLen);

    return;
}

int Sockaddr_inToString(sockaddr_in *pstSockAddr, char *szResult)
{
    char *pcTempIP = NULL;
    unsigned short nTempPort = 0;
    if ( !pstSockAddr || !szResult)
    {
        return -1;
    }
    
    pcTempIP = inet_ntoa(pstSockAddr->sin_addr);
    if ( !pcTempIP )
    {
        return -1;
    }

    nTempPort = ntohs(pstSockAddr->sin_port);

    sprintf(szResult, "%s:%d", pcTempIP, nTempPort);

    return 0;
} 

/************ 无符号加减法 ************/
unsigned int SafeAdd( unsigned int uiFirst, unsigned int uiSecond)
{
    unsigned int uiSum = uiFirst + uiSecond;
    if(uiSum < uiFirst || uiSum < uiSecond)
    {
        return UIN32_MAX;
    }

    return uiSum;
}

unsigned int SafeMinus(unsigned int uiMinuend, unsigned int uiSubtrahend)
{
    if (uiSubtrahend > uiMinuend)
    {
       return 0;
    }
    
    return uiMinuend - uiSubtrahend;
}


#endif