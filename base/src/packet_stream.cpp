#include "packet_stream.h"
#include <string.h>

CPacketEncoder::CPacketEncoder()
{
    m_aucCodeBuf = NULL;
    m_pCurCodePos = NULL;
    m_bError = false;
}

CPacketEncoder::~CPacketEncoder()
{
    m_aucCodeBuf = NULL;
    m_pCurCodePos = NULL;
    m_iMaxCodeLen = 0;
    m_bError = false;
}

int CPacketEncoder::Initialize(unsigned char * pucCodeBuf, int iMaxCodeLen)
{
    if (pucCodeBuf == NULL)
    {
        return -1;
    }

    m_aucCodeBuf = pucCodeBuf;
    m_pCurCodePos = pucCodeBuf;
    m_iMaxCodeLen = iMaxCodeLen;
    m_bError = false;
    
    return 0;
}

CPacketEncoder& CPacketEncoder::operator <<(char cChar)
{
    return (*this << (unsigned char)cChar);
}

CPacketEncoder& CPacketEncoder::operator <<(unsigned char ucChar)
{
    if (m_pCurCodePos == NULL)
    {
        return *this;
    }

    if (!CanEncodeLen((int)sizeof(unsigned char)))
    {
        return *this;
    }

    *m_pCurCodePos++ = ucChar;
    
    return *this;
}

CPacketEncoder& CPacketEncoder::operator <<(short shShort16)
{
    return (*this << (unsigned short)shShort16);
} 


CPacketEncoder& CPacketEncoder::operator <<(unsigned short ushShort16)
{
    if (m_pCurCodePos == NULL)
    {
        return *this;
    }

    if (!CanEncodeLen((int)sizeof(unsigned short)))
    {
        return *this;
    }

    *(unsigned short *)m_pCurCodePos = ushShort16;
    m_pCurCodePos += sizeof(unsigned short);
    
    return *this;
}

CPacketEncoder& CPacketEncoder::operator <<(int iInt32)
{
    return (*this << (unsigned int)iInt32);
}

CPacketEncoder& CPacketEncoder::operator <<(unsigned int uiInt32)
{
    if (m_pCurCodePos == NULL)
    {
        return *this;
    }

    if (!CanEncodeLen((int)sizeof(unsigned int)))
    {
        return *this;
    }
    
    *(unsigned int *)m_pCurCodePos = uiInt32;
    m_pCurCodePos += sizeof(unsigned int);
    
    return *this;
} 

CPacketEncoder& CPacketEncoder::operator <<(int64_t iInt64)
{
    return (*this << (uint64_t)iInt64);
}

CPacketEncoder& CPacketEncoder::operator <<(uint64_t uiInt64)
{
    if (m_pCurCodePos == NULL)
    {
        return *this;
    }

    if (!CanEncodeLen((int)sizeof(uint64_t)))
    {
        return *this;
    }

    *(uint64_t *)m_pCurCodePos = uiInt64;
    m_pCurCodePos += sizeof(uint64_t);

    return *this;
}


//编码Mem
int CPacketEncoder::EncodeMem(char *pcSrc, int iMemSize)
{
    if (m_pCurCodePos == NULL || pcSrc == NULL || iMemSize <= 0)
    {
        return -1;
    }

    if (m_pCurCodePos + (unsigned int)iMemSize > m_aucCodeBuf + m_iMaxCodeLen)
    {
        return -2;
    }

    if (!CanEncodeLen(iMemSize))
    {
        return -100;
    }

    memcpy(m_pCurCodePos, pcSrc, iMemSize);
    m_pCurCodePos += (unsigned int)iMemSize;
    
    return 0;
}

int CPacketEncoder::EncodeString( char *pcSrc, short shMaxStrLen)
{
    unsigned short ushTempLength = (unsigned short)strlen(pcSrc);
    if (ushTempLength > (unsigned short)shMaxStrLen)
    {
        ushTempLength = (unsigned short)shMaxStrLen;
    }

    if (m_pCurCodePos + (unsigned int)ushTempLength > m_aucCodeBuf + m_iMaxCodeLen)
    {
        return -2;
    }
    
    if (!CanEncodeLen((int)sizeof(unsigned short)))
    {
        return -100;
    }

    *(unsigned short *)m_pCurCodePos = ushTempLength;
    m_pCurCodePos += sizeof(unsigned short);

    if (!CanEncodeLen(ushTempLength))
    {
        return -200;
    }
    
    strncpy((char *)m_pCurCodePos, pcSrc, ushTempLength);
    if (ushTempLength == shMaxStrLen)
    {
        m_pCurCodePos[ushTempLength-1] = '\0';
    }
    
    m_pCurCodePos += (unsigned int)ushTempLength;
   
    return 0;
}

bool CPacketEncoder::CanEncodeLen(int iLen)
{
    if (m_pCurCodePos + iLen > m_aucCodeBuf + m_iMaxCodeLen)
    {
        m_bError = true;
#ifdef _DEBUG
        assert(!m_bError);
#endif
        return false;
    }
    
    return true;
}

/********************************************************************/


CPacketDecoder::CPacketDecoder()
{
    m_aucCodeBuf = NULL;
    m_pCurCodePos = NULL;
    m_bError = false;
}

CPacketDecoder::~CPacketDecoder()
{
    m_aucCodeBuf = NULL;
    m_pCurCodePos = NULL;
    m_iMaxCodeLen = 0;
    m_bError = false;
}

int CPacketDecoder::Initialize(unsigned char * pucCodeBuf, int iMaxCodeLen)
{
    if (pucCodeBuf == NULL)
    {
        return -1;
    }

    m_aucCodeBuf = pucCodeBuf;
    m_pCurCodePos = pucCodeBuf;
    m_iMaxCodeLen = iMaxCodeLen;
    m_bError = false;
    
    return 0;
}

//解码char
CPacketDecoder& CPacketDecoder::operator >>(char cChar)
{
    return (*this >> (unsigned char)cChar);
}

CPacketDecoder& CPacketDecoder::operator >>(unsigned char ucChar)
{
    if (m_pCurCodePos == NULL)
    {
        return *this;
    }

    if (!CanDecodeLen((int)sizeof(unsigned char)))
    {
        return *this;   //溢出了
    }

    ucChar = *m_pCurCodePos++;
    
    return *this;
}

CPacketDecoder& CPacketDecoder::operator >>(short shShort16)
{
    return (*this >> (unsigned short)shShort16);
} 


CPacketDecoder& CPacketDecoder::operator >>(unsigned short ushShort16)
{
    if (m_pCurCodePos == NULL)
    {
        return *this;
    }

    if (!CanDecodeLen((int)sizeof(unsigned short)))
    {
        return *this;
    }

    ushShort16 = *(unsigned short *)m_pCurCodePos;
    m_pCurCodePos += sizeof(unsigned short);
    
    return *this;
}

CPacketDecoder& CPacketDecoder::operator >>(int iInt32)
{
    return (*this >> (unsigned int)iInt32);
}

CPacketDecoder& CPacketDecoder::operator >>(unsigned int uiInt32)
{
    if (m_pCurCodePos == NULL)
    {
        return *this;
    }

    if (!CanDecodeLen((int)sizeof(unsigned int)))
    {
        return *this;
    }
    
    uiInt32 = *(unsigned int *)m_pCurCodePos;
    m_pCurCodePos += sizeof(unsigned int);
    
    return *this;
} 

CPacketDecoder& CPacketDecoder::operator >>(int64_t iInt64)
{
    return (*this >> (uint64_t)iInt64);
}

CPacketDecoder& CPacketDecoder::operator >>(uint64_t uiInt64)
{
    if (m_pCurCodePos == NULL)
    {
        return *this;
    }

    if (!CanDecodeLen((int)sizeof(uint64_t)))
    {
        return *this;
    }

    uiInt64 = *(uint64_t *)m_pCurCodePos;
    m_pCurCodePos += sizeof(uint64_t);

    return *this;
}


//解码Mem
int CPacketDecoder::DecodeMem(char *pcDest, int iMemSize)
{
    if (m_pCurCodePos == NULL || pcDest == NULL || iMemSize <= 0)
    {
        return -1;
    }

    if (!CanDecodeLen(iMemSize))
    {
        return -100;
    }

    memcpy(pcDest, m_pCurCodePos, iMemSize);
    m_pCurCodePos += (unsigned int)iMemSize;
    
    return 0;
}

int CPacketDecoder::DecodeString( char *strDst, short shMaxStrLen)
{
    if (m_pCurCodePos == NULL || strDst == NULL || shMaxStrLen <=  0 )
    {
        return -1;
    }
    
    short shStrLen = *(unsigned short *)m_pCurCodePos;
    m_pCurCodePos += sizeof(unsigned short);

    if (m_pCurCodePos + (unsigned int )shStrLen > m_aucCodeBuf + m_iMaxCodeLen)
    {
        return -2;
    }

    if (!CanDecodeLen(shStrLen))
    {
        return -200;
    }
    
    unsigned short ushRealLength = 0;
    --shMaxStrLen;

    if (shStrLen > shMaxStrLen)
    {
        ushRealLength = shMaxStrLen;
    }
    else
    {
        ushRealLength = shStrLen;
    }

    if(ushRealLength > 0)
    {
        strncpy(strDst, (char *)m_pCurCodePos, ushRealLength);
        strDst[0] = '\0';
    }

    m_pCurCodePos += shStrLen;
    
    return 0;
}

bool CPacketDecoder::CanDecodeLen(int iLen)
{
    if (m_pCurCodePos + iLen > m_aucCodeBuf + m_iMaxCodeLen)
    {
        m_bError = true;
#ifdef _DEBUG
        assert(!m_bError);
#endif
        return false;
    }
    
    return true;
}
