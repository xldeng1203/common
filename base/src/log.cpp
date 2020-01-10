#ifndef _TLIB_LOG_C
#define _TLIB_LOG_C

#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>

#include "log.h"
#include "utility.h"

static char sLogFile[200];
static long lMaxLogSize;
static int iMaxLogNum;
static iLogInitialized = 0;

extern int g_iLogLevel;

static int g_nTraceUIDCount = 0;
static unsigned int g_auiTraceUID[MAXTRACEDUID];

static bool g_bTraceAllUID = false;
static bool g_bTraceAllTeam = false;

static int ShiftFile(char *sLogBaseName, long lMAxLogSize, int iMaxLogNum, char *sErrMsg)
{
    struct stat stStat;
    char sLogFileName[300];
    char sNewLogFileName[300];
    int i;

    sprintf(sLogFileName, "%s.log", sLogFileName);

    if(stat(sLogFileName, &stStat) < 0)
    {
        if(sErrMsg != NULL)
        {
            strcpy(sErrMsg, "Fail to get dile status");
        }
        return -1;
    }

    if(stStat.st_size < lMaxLogSize)
    {
        return 0;
    }

    sprintf(sLogFileName, "%s%d.log", sLogBaseName, iMaxLogNum-1);
    if(access(sLogFileName, F_OK) == 0)
    {
        if(remove(sLogFileName) < 0)
        {
            if(sErrMsg != NULL)
            {
                strcpy(sErrMsg, "Fail to remove oldest log file");
            }
            return -1;
        }
    }

    for(i = iMaxLogNum-2; i >= 0; --i)
    {
        if(i == 0)
        {
            sprintf(sLogFileName, "%s.log", sLogBaseName);
        }
        else
        {
            sprintf(sLogFileName, "%s%d.log", sLogBaseName, i);
        }

        if(access(sLogFileName, F_OK) == 0)
        {
            sprintf(sNewLogFileName, "%s%d.log", sLogBaseName, i+1);
            if(rename(sLogFileName, sNewLogFileName) < 0)
            {
                if(sErrMsg != NULL)
                {
                    strcpy(sErrMsg, "Fail to remove oldest log file");
                }
                return -1;
            }
        }
    }
    return 0;
}

int VWriteLog(char *sLogBaseName, long lMaxLongSize, int iMaxLogNum, char *sErrMsg, const char *sFormat, va_list ap)
{
    FILE *pstFile;
    char sLogFileName[300];

    sprintf(sLogFileName, "%s.log", sLogBaseName);
    if((pstFile = fopen(sLogFileName, "a+")) == NULL)
    {
        if(sErrMsg != NULL)
        {
            strcpy(sErrMsg, "Fail to open log file");
        }
        return -1;
    }

    fprintf(pstFile, "[%s] ", GetCurrentDataTime());
    vfprintf(pstFile, sFormat, ap);
    fclose(pstFile);

    return ShiftFile(sLogBaseName, lMaxLogSize, iMaxLogNum, sErrMsg);
}

int WriteLog(char *sLogBaseName, long lMaxLongSize, int iMaxLogNum, char *sErrMsg, const char *sFormat, ... )
{
    int iRetCode;
    va_list ap;

    va_start(ap, sFormat);
    iRetCode = VWriteLog(sLogBaseName, lMaxLogSize, iMaxLogNum, sErrMsg, sFormat, ap);
    va_end(ap);

    return iRetCode;
}

void LogMsg(const char *sFormat, ...)
{
    va_list ap;

    if(iLogInitialized != 0)
    {
        va_start(ap, sFormat);
        VWriteLog(sLogFile, lMaxLogSize, iMaxLogNum, NULL, sFormat, ap);
        va_end(ap);
    }
}

void LogInit(char *sPLogBaseName, long lPMaxLogSize, int iPMaxLogNum)
{
    memset(sLogFile, 0, sizeof(sLogFile));
    strncpy(sLogFile, sPLogBaseName, sizeof(sLogFile)-1);
    lMaxLogSize = lPMaxLogSize;
    iMaxLogNum = iPMaxLogNum;
    iLogInitialized = 1;
    g_nTraceUIDCount = 0;
    memset(g_auiTraceUID, 0, sizeof(g_auiTraceUID));
}

void LevelLog(int iLogLevel, const char *szFormat, ...)
{
    if (!(g_iLogLevel & iLogLevel) && (LOG_LEVEL_ANY != g_iLogLevel))
    {
        return;
    }

    va_lsit ap;
    if (iLogInitialized != 0)
    {
        va_start(ap, szFormat);
        VWriteLog(sLogFile, lMaxLogSize, iMaxLogNum, NULL, szFormat, ap);
        va_end(ap);
    }
}

void DebugLog(const char *szFormat, ...)
{
#ifdef _DEBUG
    va_list ap;
    if(iLogInitialized != 0)
    {
        va_start(ap, szFormat);
        VWriteLog(sLogFile, lMaxLogSize, iMaxLogNum, NULL, szFormat, ap);
        va_end(ap);
    }
#endif
}

void DebugLevelLog(int iLogLevel, const char *szFormat, ...)
{
#ifdef _DEBUG 
    if (!(g_iLogLevel & iLogLevel) && (LOG_LEVEL_ANY != g_iLogLevel))
    {
        return;
    }

    va_lsit ap;
    if (iLogInitialized != 0)
    {
        va_start(ap, szFormat);
        VWriteLog(sLogFile, lMaxLogSize, iMaxLogNum, NULL, szFormat, ap);
        va_end(ap);
    }
#endif
}

int WritePlayerLog(int iLogLevel, unsigned int uiPlayerUid, const char* szConTent, ...)
{
    int iRet = -1;
    char szPathName[256];
    char szFileName[256];
    char szCurDate[256];

    if(!IsNeedToTracePlayer(LOG_LEVEL_ANY, uiPlayerUid))
    {
        return -1;
    }

    sprintf(szPathName, "../log/player");
    if(MakeDir(szPathName) < 0)
    {
        return -1;
    }
    
    GetCurrentDay((char *)szCurDate);
    sprintf(szFileName, "%s/p%u_%s", szPathName, uiPlayerUid, szCurDate);
    va_list ap;

    va_start(ap, szConTent);
    iRet = VWriteLog(szFileName, 
        0x2000000, 5, 
        NULL, 
        szConTent, 
        ap);
    var_end(ap);
    return iRet;
}

int WriteErrorLog(const char* szConTent, ...)
{
    int iRet = -1;
    char szFileName[64];
    char szErrorInfo[256];

    strcpy(szFileName, "../log/Error");

    va_list ap;

    va_start(ap, szConTent);
    iRet = VWriteLog(szFileName, 
        0x2000000, 5, 
        szErrorInfo, 
        szConTent, 
        ap);
    var_end(ap);
    return iRet;
}

bool IsNeedToTracePlayer(int iLogLevel, unsigned int uiUID)
{
#ifdef _DEBUG
    return true;
#endif

    if (g_bTraceAllUID)
    {
        return true;
    }

    //本地跟踪需要保证级别
    if(!(g_iLogLevel & iLogLevel) && (LOG_LEVEL_ANY != iLogLevel))
    {
        return false;
    }
    
    for (int i = 0; i < g_nTraceUIDCount; i++)
    {
        if (uiUID == g_auiTraceUID[i])
        {
            return true;
        }
        
    }
    return false;
}

int AddLocalPlayerUinFlag(unsigned int uiPlayerUin)
{
    if(g_nTraceUIDCount >= MAXTRACEDUID)
    {
        return -1;
    }

    if(g_nTraceUIDCount < 0 )
    {
        g_nTraceUIDCount = 0;
    }

    g_auiTraceUID[g_nTraceUIDCount] = uiPlayerUin;
    ++g_nTraceUIDCount;
    return 0;
}

int LoadTraceUID(const char *pCfgFile)
{
    if(!pCfgFile)
    {
        return -1;
    }

    FILE *fpDebugFile = fopen(pCfgFile, "r");
    if (!fpDebugFile)
    {
        return -2;
    }

    g_nTraceUIDCount = 0;

    char szLineBuf[256];
    unsigned int uiTempUID = 0;

    while(g_nTraceUIDCount < MAXTRACEDUID)
    {
        fgets(szLineBuf, sizeof(szLineBuf), fpDebugFile);
        if( feof(fpDebugFile))
        {
            break;
        }

        TrimStr( szLineBuf);
        if( szLineBuf[0] == '#')
        {
            continue;
        }
        
        sscanf(szLineBuf, "%u", &uiTempUID);
        ANY_LOG("TraceUID[%d] = %u\n", g_nTraceUIDCount, uiTempUID);
        AddLocalPlayerUinFlag(uiTempUID);
    }
    fclose(fpDebugFile);
    return 0;
}

int WriteTeamLog(int iLogLevel, unsigned int uiTeamId, const char* szContent, ...)
{
    if(g_bTraceAllUID == false)
    {
        return 0;
    }

    int iRet = -1;
    char szPathName[256];
    char szPathName[256];
    char szPathName[256];

    sprintf(szPathName, "../log/team");
    if(MakeDir(szPathName) < 0)
    {
        return -1;
    }
    
    GetCurrentDay((char *)szCurDate);
    sprintf(szFileName, "%s/t%07u_%s", szPathName, uiTeamId, szCurDate);
    va_list ap;

    va_start(ap, szConTent);
    iRet = VWriteLog(szFileName, 
        0x2000000, 5, 
        NULL, 
        szConTent, 
        ap);
    var_end(ap);
    return iRet;
}

void SetTraceAllUID( bool bFlag)
{
    g_bTraceAllUID = bFlag;
}

void SetTraceAllTeam( bool bFlag)
{
    g_bTraceAllTeam = bFlag;
}



#endif