
/*******************************************************************
** File Name:       base_define.h
** Copy Right:      neil deng
** File creator:    neil deng
** Date:            
** Version:
** Desc:            
********************************************************************/

#ifndef _BASE_DEFINE_H_
#define _BASE_DEFINE_H_

#include <string>
#include <stdarg.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

using std::string;

//tolua_begin
typedef float           f32;
typedef double          f64;
typedef unsigned char   byte;  //字节类型 8位无符号整数
typedef byte            uint8;
typedef char            int8;

typedef unsigned short  ushort; //子类型 16位无符号整数
typedef ushort          uint16;
typedef signed short    int16;
typedef unsigned int    uint;   //双子类型 32位无符号整数
typedef uint            uint32;
typedef signed long     int64;   //64位有符号整数
typedef unsigned long   uint64;  //4字类型，64位无符号整数
typedef int             int32;

typedef wchar_t         wchar;  //宽符号
typedef void *          pointer;//无类型指针
typedef uint64          datatime_t;
typedef int             SOCKET;
typedef unsigned int    lhandle_id;

//某些特定信息定义
typedef uint32          LineI;

#define CALC_PERCENT_INT_10000      10000
#define CALC_PERCENT_INT_1000       1000
#define CALC_PERCENT_INT_100        100
#define CALC_PERCENT_FLOAT_10000    10000.0f
#define CALC_PERCENT_FLOAT_1000     1000.0f
#define CALC_PERCENT_FLOAT_100      100.0f

//tolua_end

namespace CoreSocket
{
    const uint32 trunkSize = 1 * 1024 * 1024;  //64*1024; //包放大到1M
    const int warringSize = 10 * 1024;
    const uint32 unitSize = 1024;
    const uint32 bin_data_head = 128;

    const int32 T_RD_MSEC = 50;     //读超时 毫秒
    const int32 T_WR_MESC = 50;     //写超时 毫秒

    const uint32 PH_LEN = sizeof(uint32);
    const uint32 NO_LEN = sizeof(uint16);
    const uint32 MAX_HEAD = sizeof(uint32) + sizeof(uint16) + sizeof(uint16);
    const uint16 PACK_ROUND = 20;   
    const uint32 INCOMPLETE_READ = 0x00000001;      //读 未完成 标记
    const uint32 INCOMPLETE_WRITE = 0x00000002;     //写 未完成 标记

    const uint32 PACKET_MASK = trunkSize - 1;       //包含长度的整体的包大小
    const uint32 MAX_DATABUFFERSIZE = PACKET_MASK;  //包含长度的整日的包大小
    const uint32 MAX_DATASIZE = (MAX_DATABUFFERSIZE - PH_LEN);          //转发逻辑包大小 包含各种转发消息头的封装
    const uint32 MAX_USERDATASIZE = (MAX_DATASIZE - bin_data_head);     //用户逻辑包大小
    const uint32 MAX_CLIENT_REQ_DATA_SIZE = 64 * 1024;  //客户端发送数据包的大小限制到64k
    const uint32 MAX_PLAYER_DATA_SIZE = 512 * 1024; //暂定最大用户数据，为512K, 后面不够再放，应该足够用了    
}

typedef void* LZ_POINT;

#define LZ_STRSIZE(x)   x,sizeof(x)     //用在snprintf中的简化宏
#define LZ_COUNTOF(x)   (sizeof(x)/sizeof(x[0]))
#define LZ_NOTNULL(x)   (x==NULL ? "" : x)

#define LZ_MAX(a, b)    ((a) > (b) ? (a) : (b))
#define LZ_MIN(a, b)    ((a) < (b) ? (a) : (b))
#define LZ_CEIL(a, b)    ((a) == 0 ?  0 : ((a) - 1) / (b) + 1)
#define DIVIDE(a, b) ((a) / ( (b) > 0 ? (b) : 1))
#define MOD(a, b) ((a) % ( (b) > 0 ? (b) : 1))

#define LZ_UNUSED(x)        //如果 有些参数在函数类没用到，用这个可以避免警告
#define LZ_NAME(x)  #x

//从windef.h拿来的宏
//2个unsigned char 和unsigned short 的转换
#define LZ_USHORT_MAKE(high, low) ((unsigned short)(((unsigned char)((low)& 0xFF)) | (( unsigned short)((unsigned char)((high) & 0xFF))) << 8))
#define LZ_USHORT_LOW(i)          ((unsigned char)((unsigned short)(i) & )0xFF))
#define LZ_USHORT_HIGH(i)         ((unsigned char)((unsigned short)(i) >> 8))

//2个unsigned short 和unsigned int 的转换
#define LZ_UINT_MAKE(high, low) ((unsigned int)(((unsigned short)((low)& 0xFFFF)) | (( unsigned int)((unsigned short)((high) & 0xFFFF))) << 16))
#define LZ_UINT_LOW(i)          ((unsigned short)((unsigned int)(i) & )0xFFFF))
#define LZ_UINT_HIGH(i)         ((unsigned short)((unsigned int)(i) >> 16))

//2个unsigned int 和unsigned int64 的转换
#define LZ_UINT64_MAKE(high, low) ((uint64)(((unsigned int)((low)& 0xFFFFFFFF)) | ((uint64)((unsigned int)((high) & 0xFFFFFFFF))) << 32))
#define LZ_UINT64_LOW(i)          ((unsigned int)((uint64)(i) & )0xFFFFFFFF))
#define LZ_UINT64_HIGH(i)         ((unsigned int)((uint64)(i) >> 32))

//32位的位操作
//pos的取值范围是0--31
#define LZ_BIT_SET(i, pos)      ( i |= ((unsigned int)1 << pos))        //把第pos位置1
#define LZ_BIT_CLR(i, pos)      ( i &= ~((unsigned int)1 << pos))       //把第pos位置0
#define LZ_BIT_TEST(i, pos)     ((i & ((unsigned int)1 << pos)) !=0 )   //判断第pos是不是1

//1移位
#define LZ_BIT_L_MOVE_1(bit) (1 << (bit))
#define LZ_BIT_R_MOVE_1(bit) (1 >> (bit))

#define closesocket         close
#define GetNowProcessId     getpid
#define GetCurrentThreadId  pthread_self

typedef int SOCKET;

#define LZ_INVALID_SOCKET   (SOCKET)(~0)

#define lz_fopen        fopen
#define lz_stat         stat
#define lz_access       access
#define lz_rename       rename
#define lz_unlink       unlink
#define lz_remove       remove

#define lz_snprintf     snprintf
#define lz_vsnprintf    snprintf
#define lz_snwprintf    snprintf
#define lz_vsnwprintf   snprintf


//最大值常量枚举
enum
{
    LZ_PATH_MAX = 1024,         //路径的最大长度
    LZ_MAX_IPLEN = 32,          //IP地址的最大长度
    LZ_INT_MAX = 0x7FFFFFFF,     //int类型的最大值
    LZ_CONF_TXT_MAX = 1024,     //最大文本配置长度
};

#define MD5_LENGTH      32

#endif