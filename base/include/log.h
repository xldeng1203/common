/*
     File creator: neil deng
*/

#ifndef __LOG_H__
#define __LOG_H__

#include <time.h>
#include <stdarg.h>

#define LOG_LEVEL_CALL      0x0001
#define LOG_LEVEL_CFG       0x0002
#define LOG_LEVEL_TRACE     0x0004
#define LOG_LEVEL_ERR       0x0008
#define LOG_LEVEL_WARNING   0x0010
#define LOG_LEVEL_DEBUG     0x0020
#define LOG_LEVEL_MONSTER   0x0040
#define LOG_LEVEL_NONE      0x0000
#define LOG_LEVEL_ANY       -1
#define MAXTRACEDUID        256

typedef struct _targLogCfg2_
{
    char sLogFile[200];
    long lMaxLogSize;
    int iMaxLogNum;
    int iInited;
} TLogCfg2;

int VWriteLog(char *sLogBaseName, long lMaxLongSize, int iMaxLogNum, char *sErrMsg, const char *sFormat, va_list ap);
int WriteLog(char *sLogBaseName, long lMaxLongSize, int iMaxLogNum, char *sErrMsg, const char *sFormat, ... );

void LogMsg(const char *sFormat, ...);
void LogInit(char *sPLogBaseName, long lPMaxLogSize, int iPMaxLogNum);

void LevelLog(int iLogLevel, const char *szFormat, ...);
void DebugLog(const char *sFormat, ...);
void DebugLevelLog(int iLogLevel, const char *szFormat, ...);

int WritePlayerLog(int iLogLevel, unsigned int uiPlayerUid, const char* szConTent, ...);
int WritePlayerLog(unsigned int uiPlayerUid, const char* szContent, ...);

int WriteTeamLog(int iLogLevel, unsigned int uiTeamId, const char *szContent, ...);
int WriteTeamLog(unsigned int uiTeamId, const char* szContent, ...);

int WriteErrorLog(const char *szContent, ...);

bool IsNeedToTracePlayer(int iLogLevel, unsigned int uiUID);
int LoadTraceUID(const char *pCfgFile);
void SetTraceAllUID(bool bFlag);
void SetTraceAllTeam(bool bFlag);

#define LEVEL_LOG LevelLog  //推荐：按日志级别打印，可以动态关闭和打开 级别：LOG_LEVEL_*** , log.h文件上边有定义
#define ANY_LOG LogMsg      //强打日志，不能关闭，一般错误和启动日志使用
#define DEBUG_LOG DebugLog  //debug版本才有用

#define DEBUG_LEVEL_LOG DebugLevelLog
#define WRITE_LOG WriteLog  //这个不用
#define PLAYER_LOG  WritePlayerLog  //这个按uid打印到不同的文件
#define TEAM_LOG    WriteTeamLog    //按TeamId打印到不同文件
#define ERROR_LOG   WriteErrorLog   //专门打印错误日志，达到单独的一个错误文件，比较严重的在此


#endif