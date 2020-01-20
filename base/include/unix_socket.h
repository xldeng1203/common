/*******************************************************************
** File Name:       unix_socket.h
** Copy Right:      neil deng
** File creator:    neil deng
** Date:            
** Version:
** Desc:            
********************************************************************/
#ifndef _UNIXSOCKET_H_
#define _UNIXSOCKET_H_


#define UNIXSOCKET_RECV_BUFF  2097152    //2M
#define UNIXSOCKET_SEND_BUFF 2097152    

class UnixSocket
{
public:
    UnixSocket();
    ~UnixSocket();

    enum _enParse
    {
        _CLOSED = 0,
        _OPENED = 1,
        ERR_SOCK_ERROR = -1,
        ERR_SOCK_CLOSED = -2,
        ERR_SOCK_FAILED = -3,
        ERR_NO_BUFFER = -4
    };

    int SetupFD(int iFD);
    int Recv();
    int GetOnePakcet(unsigned short &ushCodeLength, unsigned char *pCode);
    int SendOnePacket(unsigned short ushCodeLength, unsigned char *pCode);

    int GetIntOnePacket(int &uiCodeLength, unsigned char *pCode);
    int sendIntOnePacket(int uiCodeLength, unsigned char *pCode);

    inline int GetSocketFD() { return m_iSocketFD; }
    inline int GetStatus() { return m_iStatus; }

    int LeftLength();
    int SendLeft();

protected:
    int Close();
    
    int m_iSocketFD;
    int m_iStatus;

private:
    int m_iReadBegin;
    int m_iReadEnd;
    unsigned char m_abyRecvBuffer[UNIXSOCKET_RECV_BUFF];

    int m_iSendBegin;
    int m_iSendEnd;
    unsigned char m_abySendBuffer[UNIXSOCKET_SEND_BUFF];

};




#endif