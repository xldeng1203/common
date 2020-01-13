/*
     File creator: neil deng
*/

#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <dlfcn.h>
#include <pthread.h>
#include <assert.h>
#include <openssl/md5.h>

/************ 时间类相关 ************/
typedef struct _tagTimeStruct_
{
    int iYear;
    int iMon;
    int iDay;
    int iHour;
    int iMinute;
    int iSecond;
} TimeStruct;

int GetTimeStruct(time_t tTime, TimeStruct *pstTime);
int TimevalPlus(timeval& tvA, timeval& tvB, timeval& tvResult);
int TimevalMinutes(timeval& tvA, timeval& tvB, timeval& tvResult);
bool IsSameHour(time_t tTime1, time_t tTime2);
bool IsSameDay(time_t tTime1, time_t tTime2);
char *GetDataTime(time_t *mytime);
char *GetCurrentDataTime(void);
int GetCurrnetDay(char *strTime);

/************ 建立目录 ************/
int MakeDir(const char *pcDirPath, pthread_mutex_t *pMutex = NULL);


/************ 字符串相关 ************/
void TromStr(char *strInput);
int Sockaddr_inToString(sockaddr_in *pstSockAddr, char *szResult);

/************ 无符号加减法 ************/
#ifndef UIN32_MAX
#define UIN32_MAX 4294967295
#endif
unsigned int SafeAdd(unsigned int uiFirst, unsigned int uiSecond);
unsigned int SafeMinus(unsigned int uiMinuend, unsigned int uiSubtrahend);

#define MYMD5KEY "#edcVFR$"

#endif