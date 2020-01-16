#ifndef _SHM_CPP_
#define _SHM_CPP_

#include "share_memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "log.h"

unsigned char* ShareMem::pbCurrentShm = NULL;

void* ShareMem::operator new(size_t nSize) throw()
{
    return (void*)pbCurrentShm;
}

void ShareMem::operator delete(void* pMem)
{
}

ShareMem::ShareMem()
{
    m_pbCurrentSegMent = pbCurrentShm + sizeof(ShareMem);
}

ShareMem::ShareMem(key_t nKey, int nSize)
{
    m_pbCurrentSegMent = pbCurrentShm + sizeof(ShareMem);
    m_nShmKey = nKey;
    m_nShmSize = nSize;
}

ShareMem::~ShareMem()
{
}

void* ShareMem::AssignMemory(size_t nSize)
{
    int nTempUsedLength = 0;
    unsigned char* pTemp = NULL;

    if (nSize <=0)
    {
        return NULL;
    }
    
    nTempUsedLength = (int)(m_pbCurrentSegMent - (unsigned char*)this);
    if (m_nShmSize - nTempUsedLength < nSize)
    {
        return NULL;
    }
    
    pTemp = m_pbCurrentSegMent;
    m_pbCurrentSegMent += nSize;

    return (void*)pTemp;
}

ShareMem* ShareMem::MakeShm(const char* szFile, char byProjID, int iSize)
{
    key_t iKey = 0;
    size_t iTempShmSize = 0;
    int iShmId = 0;

    if (!szFile)
    {
        return NULL;
    }

    iKey = ftok(szFile, byProjID);
    if (iKey < 0)
    {
        printf("Error from ftok, %s. \n", strerror(errno));
        exit(-1);
    }
    
    iTempShmSize = iSize;
    iTempShmSize += sizeof(ShareMem);

    iShmId = shmget( iKey, iTempShmSize, IPC_CREAT | IPC_EXCL | 0666);
    if (iShmId < 0)
    {
        if (errno != EEXIST)
        {
            ANY_LOG("Alloc failed, %s\n", strerror(errno));
            exit(-1);
        }

        ANY_LOG("Same shm seq (key=%08X) exist, now try to attach it... \n", iKey);        
        
        iShmId = shmget(iKey, iTempShmSize, 0666);
        if (iShmId < 0)
        {
            ANY_LOG("Attach to shm %d failed, %s. Now try to touch it\n", iShmId, strerror(errno));
            iShmId = shmget(iKey, 0, 0666);
            if (iShmId < 0)
            {
                ANY_LOG("Error, touch to shm failed, %s. \n", strerror(errno));
                exit(-1);
            }
            else
            {
                ANY_LOG("Delete exist shm %d\n", iShmId);
                if (shmctl(iShmId, IPC_RMID, NULL))
                {
                    ANY_LOG("Delete shm failed , %s \n", strerror(errno));
                    exit(-1);
                }

                iShmId = shmget(iKey, iTempShmSize, IPC_CREAT|IPC_EXCL|0666);
                if (iShmId < 0)
                {
                    ANY_LOG("Error, alloc shm failed, %s\n", strerror(errno));
                    exit(-1);
                }                
            }
        }
        else
        {
            ANY_LOG("Attach to shm succeed.\n");
        }              
    }

    ANY_LOG("ok alloced shm key = %08X, id = %d, size = %u\n", iKey, iShmId, iTempShmSize);
    
    ShareMem::pbCurrentShm = (unsigned char*)shmat(iShmId, NULL, 0);
    if ( !(ShareMem::pbCurrentShm) )
    {
        return NULL;
    }
    
    return new ShareMem(iKey, iTempShmSize);
}

int ShareMem::DeleteShm(const char* szFile, char byProjID)
{
    key_t iKey = 0;
    int iShmId = 0;

    if (!szFile)
    {
        return -1;
    }
    
    iKey = ftok(szFile, byProjID);
    if (iKey < 0)
    {
        ANY_LOG("Error from ftok, %s.\n", strerror(errno));
        return -1;
    }
    
    ANY_LOG("Touch shm key = )X%)*X... \n", iKey);

    iShmId = shmget(iKey, 0, 0666);
    if (iShmId < 0)
    {
        ANY_LOG("Error, touch shm failed, %s.\n", strerror(errno));
        return -1;
    }
    else
    {
        ANY_LOG("Now delete the exist share memory %d\n", iShmId);
        if ( shmctl(iShmId, IPC_RMID, NULL))
        {
            ANY_LOG("delete share memory failed, %s.\n", strerror(errno));
            return -1;
        }
        ANY_LOG("remove shared memory(id = %d, key = 0X%08X) succeed.\n", iShmId, iKey);
    }
    return 0;
}


#endif