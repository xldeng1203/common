
/*******************************************************************
** File Name:       packet_stream.h
** Copy Right:      neil deng
** File creator:    neil deng
** Date:            
** Version:
** Desc:            
********************************************************************/

#ifndef __PACKETSTREAM_H__
#define __PACKETSTREAM_H__

#include <stdint.h>
#include <assert.h>

#define CHECK_BUFFER_OVERFLOS(packet, uid) \
    do                                     \
    {                                      \
        if ((packet).IsError())            \
        {                                  \
            ANY_LOG("[%s], %s:%d, uid(%u) CHECK_BUFFER_OVERFLOW **********\n", __FILE__, __FUNCTION__, __LINE__, (uid));    \
            ERROR_LOG("[%s], %s:%d, uid(%u) CHECK_BUFFER_OVERFLOW **********\n", __FILE__, __FUNCTION__, __LINE__, (uid));  \
        }                                  \
    } while (0);
    
class CPacketEncoder
{
public:
    CPacketEncoder();
    ~CPacketEncoder();

public:
    int Initialize(unsigned char *pucCodeBuf, int iMaxCodeLen);
    
    inline unsigned char* GetCodeBuf() const { return m_aucCodeBuf; }

    int GetCurOffset() const
    {
        if (m_bError)
        {
            return 0x0fffffff;
        }
        return m_pCurCodePos - m_aucCodeBuf;
    }

    inline void SetCurOffset(int iOffset) { m_pCurCodePos = m_aucCodeBuf + iOffset; }

    CPacketEncoder& operator <<(char cChar);
    CPacketEncoder& operator <<(unsigned char ucChar);

    CPacketEncoder& operator <<(short shShort16);
    CPacketEncoder& operator <<(unsigned short ushShort16);

    CPacketEncoder& operator <<(int iInt32);
    CPacketEncoder& operator <<(unsigned int uiInt32);

    CPacketEncoder& operator <<(uint64_t uiInt64);
    CPacketEncoder& operator <<(int64_t iInt64);

    int EncodeMem(char *pcStr, int iMemSize);

    int EncodeString(char *pcSrc, short shMaxStrLength);

    inline bool isError() { return m_bError; }

private:
    bool CanEncodeLen(int iLen);
private:
    unsigned char* m_aucCodeBuf;
    unsigned char* m_pCurCodePos;
    int m_iMaxCodeLen;
    bool m_bError;
};

/********************************************************************/

class CPacketDecoder
{
public:
    CPacketDecoder();
    ~CPacketDecoder();

public:
    int Initialize(unsigned char* puncCodeBuf, int iMaxCodeLen);

    inline unsigned char* GetCodeBuf() const { return m_aucCodeBuf; }

    int GetCurOffset() const
    {
        if (m_bError)
        {
            return 0x0fffffff;
        }
        return m_pCurCodePos - m_aucCodeBuf;
    }

    inline void SetCurOffset(int iOffset) { m_pCurCodePos = m_aucCodeBuf + iOffset; }

    CPacketDecoder& operator >>(char cChar);
    CPacketDecoder& operator >>(unsigned char ucChar);

    CPacketDecoder& operator >>(short shShort16);
    CPacketDecoder& operator >>(unsigned short ushShort16);

    CPacketDecoder& operator >>(int iInt32);
    CPacketDecoder& operator >>(unsigned int uiInt32);

    CPacketDecoder& operator >>(uint64_t uiInt64);
    CPacketDecoder& operator >>(int64_t iInt64);

    int DecodeMem(char *pcDest, int iMemSize);

    inline bool IsError() { return m_bError; }
    //相当于GetUTF
    int DecodeString(char *strDest, short shMaxStrLength);

private:
    //判断是否能接触这么长的内容
    bool CanDecodeLen(int iLen);

private:
    int m_iMaxCodeLen; //收到的Buffer长度
    unsigned char* m_aucCodeBuf;
    unsigned char* m_pCurCodePos;
    bool m_bError;
};

#endif