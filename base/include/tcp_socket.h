/*******************************************************************
** File Name:       tcp_socket.h
** Copy Right:      neil deng
** File creator:    neil deng
** Date:            
** Version:
** Desc:            
********************************************************************/
#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <time.h>
// #include <errno.h>
// #include <signal.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <sys/time.h>
// #include <sys/file.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <dirent.h>
// #include <dlfcn.h>
// #include <pthread.h>
// #include <netdb.h>


#define DEFAULT_RECV_USER_BUFFER_SIZE 1048576    //1M
#define DEFAULT_SEND_USER_BUFFER_SIZE 2097152    //2M
#define MAX_TCPPACKETINT_SIZE   (1024*1024)

enum
{
    socket_closed = 0,
    socket_opened,
    socket_connected,
    socket_error
};

enum
{
    recv_socket_error   = -1,
    recv_no_buffer      = -2,
    recv_pair_err       = -3,
    recv_failed         = -4
};

enum
{
    send_socket_error   = -1,
    send_no_buffer      = -2,
    send_failed         = -3,
};

class TcpListener
{
public:
    TcpListener();
    ~TcpListener();

    int CreateServer(char* szIPAddr, unsigned short unPort, int iListenCount = 10);  
    int Close();  
    inline int GetSocketFD() {  return m_iSocketFD; }
    inline int GetStatus() {  return m_iStatus; }

private:
    int m_iSocketFD;
    int m_iStatus;
};

//面向服务器内部的socket封装
class TcpConnectionInt
{
public:
    TcpConnectionInt();
    ~TcpConnectionInt();

    //分配用户层的接收缓冲区和发送缓冲区
    //如果iUsrRecvBufSize为0，则自动分配默认大小的接受缓冲区
    //如果iUsrSendBufSize为0， 则不分配发送缓冲区，可以用于短链接的发送
    int AllocUserBuffer(int iUsrRecvBufSize = 0, int iUsrSendBufSize = 0);

    //作为Listener监听到的链接使用SetupAsSvrConnection
    int SetupAsSvrConnection(int iNewFD);

    //如果是主动链接，使用SetupAsClient, ConnectTo 或者 ConnectToAsClient
    int SetupAsClient(char* szLocalAddr);
    int ConnectTo(char* szIPAddr, unsigned short unPort);

    //这个直接链接域名
    int ConnectToAsClient(char* szLocalAddr, char* szIPAddr, unsigned short unPort);

    //下面是通用功能
    int Recv();
    int Close();
    int GetOnePacket(int &iPacketLength, unsigned char *pPacketBuff);
    virtual int SendOnePacket( int iPacketLength, unsigned char *pPacket);
    int SendLeft(); //如果要发送缓存下来的数据，调用
    inline int GetSocketFD() { return m_iSocketFD; }
    inline int GetStatus()  { return m_iStatus; }
    int LeftLength();

public:
    static int SetSysBuffLen(int iFD, int iSysRecvBufLen, int iSysSendBufLen);
    static int SetNonBlock(int iFD);

private:
    int m_iSocketFD;        //socket描述符
    int m_iStatus;          //连接状态

#ifdef _MULTITHREAD_      
    pthread_mutex_t m_stMutex;    //操作的互斥变量
#endif

    int m_iUsrRecvBufSize;
    int m_iUsrSendBufSize;
    int m_iReadBegin;
    int m_iReadEnd;
    unsigned char* m_abyRecvBuffer; //用户接收缓冲区
    int m_iSendBegin;
    int m_iSendEnd;
    unsigned char* m_abySendBuffer; //用户发送缓冲区
};


#endif