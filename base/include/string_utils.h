/*******************************************************************
** File Name:       string_utils.h
** Copy Right:      neil deng
** File creator:    neil deng
** Date:            2017年9月6日19:53:28
** Version:
** Desc:            
********************************************************************/


#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#include "base_define.h"

#include <stdio.h>
#include <string.h>
#include <string>
#include <stdarg.h>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

char* GetNextToken(const char *szSrc, char cSplitter, char * szToken);

//字符串常用函数类
class CSprintf
{
public:
    /*
    * Linux 和Windows的vsnprintf函数的iCount和返回值处理有些不一样，这里统一
    * @return 同windows vsnwprintf函数，缓冲区不足时返回-1
    */
    static int _lz_vsnwprintf(wchar_t* pszBuffer, int iCount, const wchar_t* pszFormat, va_list ap)
    {
        int iRet = ::vswprintf(pszBuffer, iCount, pszFormat, ap);
        if (pszBuffer == NULL && iCount == 0)
        {
            return iRet;
        }

        if (iRet >= iCount)
        {
            iRet = -1;
        }

        if (iRet > 0 && iRet < iCount)
        {
            pszBuffer[iRet] = 0;
        }
        
        va_end(ap);
        return iRet;
    }

    /*
    * Linux 和Windows的vsnwprintf函数的iCount和返回值处理有些不一样，这里统一
    * @return 同windows vsnprintf函数，缓冲区不足时返回-1
    */
    static int _lz_vsnprintf(char* pszBuffer, int iCount, const char* pszFormat, va_list ap)
    {
        int iRet = ::vsnprintf(pszBuffer, iCount, pszFormat, ap);
        if (pszBuffer == NULL && iCount == 0)
        {
            return iRet;
        }

        if (iRet >= iCount)
        {
            iRet = -1;
        }

        if (iRet > 0 && iRet < iCount)
        {
            pszBuffer[iRet] = 0;
        }
        
        va_end(ap);
        return iRet;
    }

    /*
    * Linux 和Windows的snprintf函数的iCount和返回值处理有些不一样，这里统一
    * @return 同windows vsnprintf函数，缓冲区不足时返回-1
    */
    static int _lz_snprintf(char* pszBuffer, int iCount, const char* pszFormat, ...)
    {
        va_list ap;
        va_start(ap, pszFormat);
        int iRet = _lz_vsnprintf(pszBuffer, iCount, pszFormat, ap);
        va_end(ap);
        return iRet;
    }

    /*
    * Linux 和Windows的snwprintf函数的iCount和返回值处理有些不一样，这里统一
    * @return 同windows vsnprintf函数，缓冲区不足时返回-1
    */
    static int _lz_snwprintf(wchar_t* pszBuffer, int iCount, const wchar_t* pszFormat, ...)
    {
        va_list ap;
        va_start(ap, pszFormat);
        int iRet = _lz_vsnwprintf(pszBuffer, iCount, pszFormat, ap);
        va_end(ap);
        return iRet;
    }
};


///字符串常用函数类
class CStringUtils
{
public:
    /**
    * 格式化字符串.
    * 根据参数pszFormat字符串来转换并格式化数据，然后将结果复制到参数str所指的字符串中，直到字符串结束为止
    * @return 返回格式化后字符串的长度
    */
    static int Format(string& str, const char* pszFormat, ...)
    {
        va_list ap;
        va_start(ap, pszFormat);
        int iRet = FormatV(str, pszFormat, ap);
        va_end(ap);
        return iRet;
    }

    /**
    * 字符替换
    * @return 返回被替换的次数
    */
    static int Replace(string& str, char chOld, char chNew)
    {
        if (chOld == chNew)
        {
            return 0;
        }

        int iCount = 0;
        for (unsigned int i = 0; i < str.length(); ++i)
        {
            if (str[i] == chOld)
            {
                str[i] = chNew;
                ++iCount;
            }
        }

        return iCount;
    }

    /**
    * 字符串替换
    * @return 返回被替换的次数
    */
    static int Replace(string& str, const char* pszOld, const char* pszNew)
    {
        if (pszOld == pszNew)
        {
            return 0;
        }

        const size_t iOldLen = strlen(pszOld);
        const size_t iNewLen = strlen(pszNew);
        int iCount = 0;
        size_t iIndex = str.find(pszOld, 0);
        while (iIndex != string::npos)
        {
            str.erase(iIndex, iOldLen);
            str.insert(iIndex, pszNew);
            iIndex = str.find(pszOld, iIndex + iNewLen);
            ++iCount;
        }

        return iCount;
    }

    /**
    * 删除字符串中的某字符
    * @return 返回删除的次数
    */
    static int Remove(string& str, char ch)
    {
        size_t iIndex = str.find(ch);
        int iCount = 0;
        while (iIndex != string::npos)
        {
            str.erase(iIndex, 1);
            iIndex = str.find(ch);
            ++iCount;
        }

        return iCount;
    }

    /**
    * 删除字符串中的某些字符
    * @return 返回删除的次数
    */
    static int Remove(string& str, const char* pszTrim)
    {
        size_t iIndex = str.find_first_of(pszTrim);
        int iCount = 0;
        while (iIndex != string::npos)
        {
            str.erase(iIndex, 1);
            iIndex = str.find_first_of(pszTrim);
            ++iCount;
        }

        return iCount;
    }

    /**
    * 删除字符串中某些字符外的字符.
    * @return 返回删除的次数
    */
    static int RemoveNot(string& str, const char* pszTrim)
    {
        size_t iIndex = str.find_first_not_of(pszTrim);
        int iCount = 0;
        while (iIndex != string::npos)
        {
            str.erase(iIndex, 1);
            iIndex = str.find_first_not_of(pszTrim);
            ++iCount;
        }

        return iCount;
    }

    /**
    * 转为大写
    */
    static string& MakeUpper(string& str)
    {
        transform(str.begin(), str.end(), str.begin(), static_cast<int(*)(int)>(toupper));
        return str;
    }

    /**
    * 转为小写
    */
    static string & MakeLower(string& str)
    {
        transform(str.begin(), str.end(), str.begin(), static_cast<int(*)(int)>(tolower));
        return str;
    }

    /**
    * 字符串比较，不区分大小写
    */
    static int CompareNoCase(const string& str1, const string& str2)
    {
        return strcasecmp(str1.c_str(), str2.c_str());
    }

    /**
    * 比较前count个字符
    */
    static int NCompareCase(const string& str1, const string& str2, size_t count)
    {
        return strncmp(str1.c_str(), str2.c_str(), count);
    }

    /**
    * 比较前count个字符，不区分大小写
    */
    static int NCompareNoCase(const string& str1, const string& str2, size_t count)
    {
        return strncasecmp(str1.c_str(), str2.c_str(), count);
    }

    /**
    * 删除字符串左边中的某些字符
    */
    static string& TrimLeft(string& str, const char* pszTrim)
    {
        size_t iIndex = str.find_first_not_of(pszTrim);
        if (iIndex != string::npos)
        {
            str.erase(0, iIndex);
        }

        return str;
    }

    /**
    * 删除字符串右边中的某些字符
    */
    static string& TrimRight(string& str, const char* pszTrim)
    {
        size_t iIndex = str.find_last_not_of(pszTrim);
        if ((++iIndex) != str.length())
        {
            str.erase(iIndex, str.length() - iIndex);
        }

        return str;
    }

    /**
    * 取左边iCount个字符的字符串
    */
    static string Left(const string& str, int iCount)
    {
        return str.substr(0, iCount);
    }

    /**
    * 取右边iCount个字符的字符串
    */
    static string Right(const string& str, int iCount)
    {
        return str.substr(str.length() - iCount, iCount);
    }

    /**
    * 格式化字符串.
    * 根据参数pszFormat字符串来转换并格式化数据，然后将结果复制到参数str所指的字符串中，直到字符串结束为止
    * @return 返回格式化后字符串的长度
    */
    static int FormatV(string& str, const char* pszFormat, va_list ap)
    {
        va_list apcopy;

        va_copy(apcopy, ap);
        //apcopy = ap;

        int iSize = CSprintf::_lz_vsnprintf(NULL, 0, pszFormat, ap);   // 计算需要的内存空间
        if (iSize <= 0)
        {
            return iSize;
        }
        ++iSize;

        int iLen = 0;
        char* pszBuf = new char[iSize];
        if (!pszBuf)
        {
            return -1;
        }
        iLen = CSprintf::_lz_vsnprintf(pszBuf, iSize, pszFormat, apcopy);
        va_end(apcopy);

        if (iLen < 0 || iLen >= iSize)
        {
            delete[] pszBuf;
            return iLen;
        }

        pszBuf[iLen] = 0;
        str.assign(pszBuf, iLen);
        delete[] pszBuf;

        return iLen;
    }

    /**
    * 十进制整数转换校验函数.
    * 1. 要求格式[whitespace][sign]digits[whitespace]\n
    * 2. NULL和空串都会返回false\n
    * 3. 含有非+-0123456789字符会返回false,如果数据类型有nusigned 前缀则+-号也为非法字符\n
    * 4. 数值范围在实际转换的时候由转换函数来校验，本函数不对其作处理
    */
    static bool StrToIntVerify(const char *pszStr, bool Isunsigned = false)
    {
        char* p = (char*)pszStr;
        if (p == NULL || *p == 0)
        {
            return false;
        }

        // 忽略空白
        while (*p == ' ')
        {
            ++p;
        }

        //int sign = 0; // -1表示以负号开始 1表示以加号开始 0表示以数字开始
        if ((*p == '-' || *p == '+') && Isunsigned)
        {
            return false;
        }
        ++p;

        for (; *p; p++)
        {
            if (*p >= '0' && *p <= '9')
                continue;
            else
                if (*p == ' ')
                { // 遇到空格, 则后续都必须是空格才合法
                for (; *p != 0; ++p)
                {
                    if (*p != ' ')
                    {
                        return false;
                    }
                }
                }
                else
                    return false;
        }
        return true;

    }

    /**
    * 严格的十进制整数转换模扳函数.
    * 1. 支持格式[whitespace][sign]digits[whitespace]\n
    * 2. @参数iLen为待转化的字符个数，-1表示不定长以\0作为结束标志\n
    * 3. 返回值为转换后的数字，如果转换失败的话返回0，错误码由piRet接收\n
    * 4. NULL和空串错误码为-1\n
    * 5. 含有非+-0123456789字符会错误码为-2\n
    * 6. 超过数值范围错误码为-3\n
    */
    template<typename T>
    static T StrToInt(const char* pszStr, int iLen = -1, int* piRet = NULL)
    {
        const char* pszEnd = (iLen > 0 ? pszStr + iLen - 1 : NULL);
        char* p = (char*)pszStr;
        if (p == NULL || *p == 0)
        {
            if (piRet)
            {
                *piRet = -1;
            }
            return 0;
        }

        const bool bIsUnsigned = (((T)(-1)) > 0);
        T basedata = 1;
        T makedata = (T)-1;
        const T AbsMax = bIsUnsigned ? makedata : (basedata << (sizeof(T) * 8 - 1) ^ makedata);
        //        const T AbsMax = bIsUnsigned ? (T)(-1) : (T)((((T)(1))<<(GETTYPEBITSIZE(T)-1))^((T)(-1)));
        //        const T AbsMax = bIsUnsigned ? (T)(-1) : (T)(((T)(1))<<(sizeof(T)*8-1));
        const T Overflow = AbsMax / 10;

        // 忽略空白
        while (*p == ' ' && (pszEnd ? p <= pszEnd : true))
        {
            ++p;
        }

        int sign = 0; // -1表示以负号开始 1表示以加号开始 0表示以数字开始
        if (*p == '-' || *p == '+')
        {
            if (bIsUnsigned)
            {
                if (piRet)
                {
                    *piRet = -2;
                }
                return 0;
            }

            sign = (*p == '-' ? -1 : 1);
            ++p;
        }

        // 忽略0
        while (*p == '0' && (pszEnd ? p <= pszEnd : true))
        {
            ++p;
        }

        T ret = 0;
        for (; *p != 0 && (pszEnd ? p <= pszEnd : true); ++p)
        {
            if (*p >= '0' && *p <= '9')
            {
                if (ret > Overflow) // 整型溢出了
                {
                    if (piRet)
                    {
                        *piRet = -3;
                    }
                    return 0;
                }
                else if (ret == Overflow)
                {
                    if (bIsUnsigned)
                    {
                        if (*p > '5')
                        {
                            if (piRet)
                            {
                                *piRet = -3;
                            }
                            return 0;
                        }
                    }
                    else
                    {
                        if ((sign < 0 && *p > '8') || (sign >= 0 && *p > '7'))
                        {
                            if (*p > '5')
                            {
                                if (piRet)
                                {
                                    *piRet = -3;
                                }
                                return 0;
                            }
                        }
                    }
                }

                ret = ret * 10 + (*p - '0');
            }
            else if (*p == ' ')
            {
                // 遇到空格, 则后续都必须是空格才合法
                for (; *p != 0 && (pszEnd ? p <= pszEnd : true); ++p)
                {
                    if (*p != ' ')
                    {
                        if (piRet)
                        {
                            *piRet = -2;
                        }
                        return 0;
                    }
                }
                break;
            }
            else
            {
                if (piRet)
                {
                    *piRet = -2;
                }
                return 0;
            }
        }
        if (piRet)
        {
            *piRet = 0;
        }
        return (sign < 0 ? 0 - ret : ret);
    }

    /**
    * 分割字符串的相关操作.
    * 把字符串分割成多个字符串，可以有多个分割符
    * @note 这种方式会忽略重复的分割符
    */
    static void Split(const string& strBuf, const string& strDel, std::vector<string>& aryStringList)
    {
        size_t pos1 = 0, pos2 = 0;
        while (true)
        {
            pos1 = strBuf.find_first_not_of(strDel, pos2);
            if (pos1 == string::npos)
            {
                break;
            }

            pos2 = strBuf.find_first_of(strDel, pos1);
            if (pos2 == string::npos)
            {
                aryStringList.push_back(strBuf.substr(pos1));
                break;
            }
            else
            {
                aryStringList.push_back(strBuf.substr(pos1, pos2 - pos1));
            }
        }
    }

    /**
    * 分割字符串的相关操作.
    * 把字符串分割成多个字符串，分隔符唯一
    * @note 这种方式会#不#忽略重复的分割符
    */
    static void SplitNoSkip(const string& strBuf, const string& strDel, std::vector<string>& aryStringList)
    {
        size_t pos1 = 0, pos2 = 0;
        while (true)
        {
            pos2 = strBuf.find(strDel, pos1);
            if (pos2 == string::npos)
            {
                aryStringList.push_back(strBuf.substr(pos1));
                break;
            }
            else
            {
                aryStringList.push_back(strBuf.substr(pos1, pos2 - pos1));
                pos1 = pos2 + strDel.size();
            }
        }
    }

    /**
    * 分割name=value串
    * @return 分割成功返回true
    */
    static bool SplitIni(const string& strLine, string& strName, string& strValue, char chDel = '=')
    {
        if (strLine.size() == 0)
        {
            return false;
        }

        size_t iIndex = strLine.find(chDel);
        if (iIndex == string::npos)
        {
            return false;
        }

        strName = strLine.substr(0, iIndex);
        strValue = strLine.substr(iIndex + 1, strLine.size() - iIndex - 1);

        return true;
    }

    //请用lzconv库，对中文支持比iconv库更好
#if 0
    /**
    * 字符串编码
    */
    static int Iconv(char *from_charset, char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t outlen)
    {
#ifdef LZ_OS_LINUX
        if (from_charset == NULL || to_charset == NULL)
        {
            return -1;
        }

        if (inbuf == NULL || outbuf == NULL)
        {
            return -2;
        }

        if (0 == inlen || outlen == 0)
        {
            return -3;
        }

        iconv_t   cd;
        char   **pin   =   &inbuf;
        char   **pout   =   &outbuf;

        cd = iconv_open(to_charset,from_charset);
        if ((iconv_t)-1 == cd)
        {
            return -4;
        }

        size_t curLen = inlen;
        size_t curOutLen = outlen - 1;
        while (iconv(cd, (char **)pin, (size_t*)&curLen, pout, (size_t*)&curOutLen)==(size_t)-1)
        {
            if (E2BIG == errno) //outbuf不够
            {
                return -5;
            }
            *outbuf = *inbuf;
            inbuf++;
            outbuf++;
            curLen--;
            curOutLen--;
        }

        *outbuf = '\0';

        iconv_close(cd);
#endif
        return 0;
    }

    static int GbToUtf8(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
    {
        return Iconv("gb2312", "utf-8", inbuf, inlen, outbuf, outlen);
    }

    static int Utf8ToGb(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
    {
        return Iconv("utf-8", "gb2312", inbuf, inlen, outbuf, outlen);
    }
#endif

    /**
    * 把0-9,A-F这样的十六进制字符转为相应的十进制
    */
    static int HexToInt(char ch)
    {
        if(ch >= '0' && ch <= '9')
        {
            return ch - '0';
        }
        else if (ch >= 'a' && ch <= 'f')
        {
            return ch - 'a' + 10;
        }
        else if (ch >= 'A' && ch <= 'F')
        {
            return ch - 'A' + 10;
        }
        else
        {
            return 0;
        }
        }

    /**
    * Url编码
    */
    static std::string& UrlEncode(const std::string& strSrc, std::string& strDst)
    {
        return UrlEncode(strSrc.c_str(), strDst);
    }

    static std::string& UrlEncode(const char* pszSrc, std::string& strDst)
    {
        const size_t iLen = strlen(pszSrc);

        strDst = "";
        strDst.reserve(iLen * 3);

        char sz[8];
        for (size_t i = 0; i < iLen; ++i)
        {
            if ((pszSrc[i] >= '0' && pszSrc[i] <= '9') ||
                (pszSrc[i] >= 'a' && pszSrc[i] <= 'z') ||
                (pszSrc[i] >= 'A' && pszSrc[i] <= 'Z') ||
                (pszSrc[i] == '-' || pszSrc[i] == '.' || pszSrc[i] == '_'))
            {
                strDst += pszSrc[i];
            }
            //腾讯开放平台使用的URL编码中把空格也编为%20
            //else if (pszSrc[i] == ' ')
            //{
            //    strDst += '+';
            //}
            else
            {
                CSprintf::_lz_snprintf(sz, sizeof(sz), "%%%02X", (unsigned char)(pszSrc[i]));
                strDst += sz;
            }
        }

        return strDst;
    }

    /**
    * Url解码
    */
    static bool UrlDecode(const std::string& strSrc, std::string& strDst)
    {
        return UrlDecode(strSrc.c_str(), strDst);
    }

    static bool UrlDecode(const char* pszSrc, std::string& strDst)
    {
        const size_t iLen = strlen(pszSrc);

        strDst = "";
        strDst.reserve(iLen);
        char ch;
        for (size_t i = 0; i < iLen;)
        {
            switch (pszSrc[i])
            {
            case '+':
                strDst += ' ';
                ++i;
                break;

            case '%':
                if ((i + 2) >= iLen)
                {
                    return false;
                }

                ch = char((HexToInt(pszSrc[i + 1]) << 4) + HexToInt(pszSrc[i + 2]));
                strDst += ch;
                i += 3;
                break;

            default:
                strDst += pszSrc[i];
                ++i;
                break;
            }
        }

        return true;
    }

    /**
    * 分割Url字符串
    * Url串中的特殊字符需要在调用前过滤
    */
    static bool SplitUrl(const std::string& strLine, std::map<std::string, std::string>& mapNameValue,
        bool bEndecode = true, bool bNameCase = false)
    {
        std::vector<std::string> aryList;
        Split(strLine, "&", aryList);

        std::string strName, ssName;
        std::string strValue, ssValue;
        for (unsigned int i = 0; i<aryList.size(); ++i)
        {
            if (aryList[i].size() == 0)
            {
                continue;
            }

            if (!SplitIni(aryList[i], strName, strValue))
            {
                return false;
            }

            if (bEndecode)
            {
                if (!UrlDecode(strName.c_str(), ssName) ||
                    !UrlDecode(strValue.c_str(), ssValue))
                {
                    return false;
                }
            }
            else
            {
                ssName = strName;
                ssValue = strValue;
            }

            if (!bNameCase)
            {
                MakeLower(ssName);
            }

            mapNameValue[ssName] = ssValue;
        }
        return true;
    }
};

#endif