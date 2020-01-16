#include "string_utils.h"
#include "utility.h"

char* GetNextToken(const char *szSrc, char cSplitter, char *szToken)
{
    char *pcPos = NULL;
    int iTokenLength = 0;

    if ( !szSrc || szSrc[0] == '\0')
    {
        if (szToken)
        {
            szSrc[0] = '\0';
        }
        
        return NULL;
    }

    pcPos = strchr((char*)szSrc, (int)cSplitter);

    if (pcPos)
    {
        if (szToken)
        {
            iTokenLength = (int)(pcPos - szSrc);
            memccpy((void *)szToken, (const void*)szSrc, iTokenLength);
            szToken[iTokenLength] = '\0';
        }
        pcPos++;
    }
    else
    {
        if (szToken)
        {
            strcpy(szToken, szSrc);
        }
    }
    
    return pcPos;
}