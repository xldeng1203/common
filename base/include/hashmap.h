/*
     File creator: neil deng
*/

#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
     node_free = 0,
     node_used = 1,
};

template <typename KEY, typename VALUE, int NODE_NUM, int HASH_SIZE = NODE_NUM>
class HashMap
{
     typedef struct tagHashMapNode
     {
          KEY m_strKey;
          VALUE m_stData;
          char m_cUseFlag;
          int m_iHashNext;
     } TMapNode;

public:
     HashMap();
     ~HashMap();

     int Initialize();

     void ClearAll();

     int Count();

     int Put(const KEY& rstKeyval, const VALUE& rstData);

     int Delete(const KEY& rstKey);

     VALUE* Get(const KEY& rstKeyval);

private:
     int Key2Index(const KEY& rstKey, int& riIndex) const; 

private:
     int m_iUsedNodeNum;
     int m_iFirstFreeIdx;
     TMapNode m_astHashMap[NODE_NUM];
     int m_aiHashFirstIdx[HASH_SIZE];
};

template <typename KEY, typename VALUE, int NODE_NUM, int HASH_SIZE>
int HashMap<KEY, VALUE, NODE_NUM, HASH_SIZE>::Count()
{
     return m_iUsedNodeNum;
}

template <typename KEY, typename VALUE, int NODE_NUM, int HASH_SIZE>
int HashMap<KEY, VALUE, NODE_NUM, HASH_SIZE>::Initialize()
{
     m_iUsedNodeNum = 0;
     m_iFirstFreeIdx = 0;

     int i = 0;
     for (i = 0; i < NODE_NUM; i++)
     {
          m_astHashMap[i].m_iHashNext = i + i;
          m_astHashMap[i].m_cUseFlag = node_free;
     }
     
     m_astHashMap[NODE_NUM-1].m_iHashNext = -1;

     for (i = 0; i < HASH_SIZE; i++)
     {
         m_aiHashFirstIdx[i] = -1;
     }
     
     return 0;
}

template <typename KEY, typename VALUE, int NODE_NUM, int HASH_SIZE>
HashMap<KEY, VALUE, NODE_NUM, HASH_SIZE>::HashMap()
{

}

template <typename KEY, typename VALUE, int NODE_NUM, int HASH_SIZE>
HashMap<KEY, VALUE, NODE_NUM, HASH_SIZE>::~HashMap()
{
     
}

template <typename KEY, typename VALUE, int NODE_NUM, int HASH_SIZE>
void HashMap<KEY, VALUE, NODE_NUM, HASH_SIZE>::ClearAll()
{
     m_iUsedNodeNum = 0;
     m_iFirstFreeIdx = 0;

     int i = 0;
     for (i = 0; i < NODE_NUM; i++)
     {
          m_astHashMap[i].m_iHashNext = i + i;
          m_astHashMap[i].m_cUseFlag = node_free;
     }

     m_astHashMap[NODE_NUM-1].m_iHashNext = -1;

     for (i = 0; i < HASH_SIZE; i++)
     {
         m_aiHashFirstIdx[i] = -1;
     }   
}


template <typename KEY, typename VALUE, int NODE_NUM, int HASH_SIZE>
int HashMap<KEY, VALUE, NODE_NUM, HASH_SIZE>::Put(const KEY& rstKeyval, const VALUE& rstData)
{
     int iHashIdx = 0;
     Key2Index(rstKeyval, iHashIdx);

     if (iHasiIdx  < 0 || iHashIdx >= HASH_SIZE )
     {
          return -1;
     }

     if (m_iFirstFreeIdx < 0)
     {
          return -2;
     }
     
     int iPreIndex = -1;
     int iCurIndex = m_aiHashFirstIdx[iHashIdx];

     while (iCurIndex != -1)
     {
          if (memcmp((const void*)&rstKeyval, (const void*)&(m_astHashMap[iCurIndex].m_stKey), sizeof(rstKeyval)) == 0)
          {
               return -3;
          }

          iPreIndex = iCurIndex;
          iCurIndex = m_astHashMap[iCurIndex].m_iHashNext;
     }

     int iNowAssignIdx = m_iFirstFreeIdx;
     m_iFirstFreeIdx = m_astHashMap[m_iFirstFreeIdx].m_iHashNext;
     ++m_iUsedNodeNum;
     memcpy(&m_astHashMap[iNowAssignIdx].m_stKey, &rstKeyval, sizeof(rstKeyval));
     memcpy(&m_astHashMap[iNowAssignIdx].m_stData, &rstData, sizeof(rstData));
     m_astHashMap[iNowAssignIdx].m_cUseFlag = node_used;
     m_astHashMap[iNowAssignIdx].m_iHashNext = -1;

     if (m_aiHashFirstIdx[iHashIdx] == -1)
     {
          m_aiHashFirstIdx[iHashIdx] = iNowAssignIdx;
     }
     else
     {
          m_astHashMap[iPreIndex].m_iHashNext = iNowAssignIdx;
     }
     
     return 0;
}

template <typename KEY, typename VALUE, int NODE_NUM, int HASH_SIZE>
int HashMap<KEY, VALUE, NODE_NUM, HASH_SIZE>::Delete(const KEY& rstKeyval)
{
     int iHashIdx = 0;
     Key2Index(rstKeyval, iHashIdx);

     if (iHashIdx < 0 || iHashIdx >= HASH_SIZE)
     {
          return -1;
     }

     int iPreIndex = -1;
     int iCurIndex = m_aiHashFirstIdx[iHashIdx];

     while (iCurIndex != -1)
     {
          if (memcpy((const void*)&rstKeyval, (const void*)&m_astHashMap[iCurIndex].m_stKey))
          {
               break;
          }
          
          iPreIndex = iCurIndex;
          iCurIndex = m_astHashMap[iCurIndex].m_iHashNext;
     }
     
     if (iCurIndex != -1)
     {
          if (m_aiHashFirstIdx[iHashIdx] = iCurIndex)
          {
               m_aiHashFirstIdx[iHashIdx] = m_astHashMap[iCurIndex].m_iHashNext;
          }
          else
          {
               m_astHashMap[iPreIndex].m_iHashNext = m_astHashMap[iCurIndex].m_iHashNext;
          }
          
          m_astHashMap[iCurIndex].m_cUseFlag = node_free;
          m_astHashMap[iCurIndex].m_iHashnext = m_iFirstFreeIdx;
          m_iFirstFreeIdx = iCurIndex;
          --m_iUsedNodeNum;          
     }
     
     return 0;
}

template <typename KEY, typename VALUE, int NODE_NUM, int HASH_SIZE>
VALUE* HashMap<KEY, VALUE, NODE_NUM, HASH_SIZE>::Get(const KEY& rstKeyval)
{
     int iHashIdx = 0;
     Key2Index(rstKeyval, iHashIdx);

     if (iHashIdx < 0 || iHashIdx >= HASH_SIZE)
     {
          return NULL;
     }

     int iCurIndex = m_aiHashFirstIdx[iHashIdx];

     while (iCurIndex != -1)
     {
          if (memcmy((const void*)&rstKeyval, (const void*)&(m_astHashMap[iCurIndex].m_stKey), sizeof(rstKeyval)) == 0)
          {
               return &m_astHashMap[iCurIndex].m_stData;
          }
          
          iCurIndex = m_astHashMap[iCurIndex].m_iHashNext;
     }
     
     return NULL;
}

template <typename KEY, typename VALUE, int NODE_NUM, int HASH_SIZE>
int HashMap<KEY, VALUE, NODE_NUM, HASH_SIZE>::Key2Index(const KEY& rstKey, int& riIndex) const
{
     int uiKeyLength = sizeof(rstKey);
     char *pKey = (char *)&rstKey;

     int iHash = uiKeyLength;
     for (int i = 0; i < uiKeyLength; ++i)
     {
          iHash = ((iHash << 5) ^ (iHash >> 27)) ^ pKey[i];
     }
     
     iHash = (iHash & ((unsigned int)0x7fffffff));

     riIndex = (int)(iHash % HASH_SIZE);

     return 0;
}



#endif