/*
     File creator: neil deng
*/

#ifndef _SHARE_MEMORY_H_
#define _SHARE_MEMORY_H_

#include <sys/ipc.h>
#include <sys/shm.h>

class ShareMem 
{
public:
     ShareMem ();
     ShareMem (key_t iShmKey, int nSize);
     ~ShareMem();

     void* operator new(size_t nSize) throw();
     void operator delete(void* pMem);

     void* AssignMemory(size_t nSize);
     static unsigned char* pbCurrentShm;

     key_t m_nShmKey;
     size_t m_nShmSize;
     unsigned char* m_pbCurrentSegMent;

     static ShareMem* MakeShm(const char* szFile, char byProjID, int iSize);
     static int DeleteShm(const char* szFile, char byProjID);
};


#endif