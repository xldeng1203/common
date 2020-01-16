/*
     File creator: neil deng
*/

#ifndef _OBJQUEUE_H_
#define _OBJQUEUE_H_

#define ITEM_NOT_USED    0
#define ITEM_USED        1

typedef struct 
{
     int m_iNextItem;
     int m_PrevItem;
     char m_cUseFlag;
} TQueueItem;

template <int MAXQUEUELENGTH>
class ObjQueue
{
public:
     ObjQueue();
     ~ObjQueue();

     int Initialize();

     int GetQueueLength();

     int GetHead();

     int PopHead();

     int Delete(int iItemidx);

     int Append(int iItemIdx);

     int GetNext(int iItemIdx, int& iNextItemIdx);

     int GetPrev(int iItemIdx, int& iPrevItemIdx);

private:
     int SetNext(int iItemIdx, int iNextItemIdx);
     int SetPrev(int iItemIdx, int iPrevItemIdx);

private:
     int m_iCurCount;
     int m_iHeadIdx;
     int m_iTailIdx;
     TQueueItem m_astQueueItems[MAXQUEUELENGTH];
};

template <int MAXQUEUELENGTH>
ObjQueue<MAXQUEUELENGTH>::ObjQueue()
{

}

template <int MAXQUEUELENGTH>
ObjQueue<MAXQUEUELENGTH>::~ObjQueue()
{

}

template <int MAXQUEUELENGTH>
int ObjQueue<MAXQUEUELENGTH>::Initialize()
{
     m_iHeadIdx = -1;
     m_iTailIdx = -1;
     m_iCurCount = 0;

     for (int  i = 0; i < MAXQUEUELENGTH; ++i)
     {
          m_astQueueItems[i].m_iNextItem = -1;
          m_astQueueItems[i].m_iPrevItem = -1;
          m_astQueueItems[i].m_cUseFlag = ITEM_NOT_USED;
     }
     
     return 0;
}

template <int MAXQUEUELENGTH>
int ObjQueue<MAXQUEUELENGTH>::GetQueueLength()
{
     return m_iCurCount;
}

template <int MAXQUEUELENGTH>
int ObjQueue<MAXQUEUELENGTH>::GetHead()
{
     return m_iHeadIdx;
}

template <int MAXQUEUELENGTH>
int ObjQueue<MAXQUEUELENGTH>::PopHead()
{
     int iTempItem = m_iHeadIdx;

     if (m_iHeadIdx != -1)
     {
          Delete(m_iHeadIdx);
     }
     
     return iTempItem;
}

template <int MAXQUEUELENGTH>
int ObjQueue<MAXQUEUELENGTH>::Delete(int iItemIdx)
{
     if (iItemIdx < 0 || iItemIdx >= MAXQUEUELENGTH)
     {
          return -1;
     }

     if (m_astQueueItems[iItemIdx].m_cUseFlag == ITEM_NOT_USED)
     {
          m_astQueueItems[iItemIdx].m_iPrevItem = -1;
          m_astQueueItems[iItemIdx].m_iNextItem = -1;
          return 0;
     }
     
     int iPrevItemIdx = m_astQueueItems[iItemIdx].m_iPrevItem;
     int iNextItemIdx = m_astQueueItems[iItemIdx].m_iNextItem;
     
     if (iPrevItemIdx == -1)
     {
          if (iItemIdx != m_iHeadIdx)
          {
               return 3;
          }
          
          m_iHeadIdx = iNextItemIdx;
     }
     else
     {
          SetNext(iPrevItemIdx, iNextItemIdx);
     }

     if (iNextItemIdx == -1)
     {
          if (iItemIdx != m_iTailIdx)
          {
               return -4;
          }
          
          m_iTailIdx = iPrevItemIdx;
     }
     else
     {
          SetPrev(iNextItemIdx, iPrevItemIdx);
     }
     
     m_astQueueItems[iItemIdx].m_iPrevItem = -1;
     m_astQueueItems[iItemIdx].m_iNextItem = -1;
     m_astQueueItems[iItemIdx].m_cUseFlag = ITEM_NOT_USED;

     --m_iCurCount;
     
     return 0;
}

template <int MAXQUEUELENGTH>
int ObjQueue<MAXQUEUELENGTH>::Append(int iItemIdx)
{
     if (iItemIdx < 0 || iItemIdx >= MAXQUEUELENGTH)
     {
          return -1;
     }
     

     if (m_iTailIdx == iItemIdx)
     {
          return 0;
     }
     
     if (m_astQueueItems[iItemIdx].m_cUseFlag == ITEM_USED)
     {
          Delete(iItemIdx);
     }
     
     m_astQueueItems[iItemIdx].m_iPrevItem = m_iTailIdx;
     m_astQueueItems[iItemIdx].m_iNextItem = -1;
     m_astQueueItems[iItemIdx].m_cUseFlag = ITEM_USED;

     if (m_iTailIdx == -1)
     {
          m_iTailIdx = m_iHeadIdx = iItemIdx;
     }
     else
     {
          SetNext(m_iTailIdx, iItemIdx);          
          m_iTailIdx = iItemIdx;
     }
     
     return 0;
}

template <int MAXQUEUELENGTH>
int ObjQueue<MAXQUEUELENGTH>::GetNext(int iItemIdx, int& iNextItemIdx)
{
     if (iItemIdx < 0 || iItemIdx >= MAXQUEUELENGTH)
     {
          return -1;
     }
     
     if (m_astQueueItems[iItemIdx].m_cUseFlag == ITEM_NOT_USED)
     {
          return -3;
     }

     iNextItemIdx = m_astQueueItems[iItemIdx].m_iNextItem;

     return 0;
}

template <int MAXQUEUELENGTH>
int ObjQueue<MAXQUEUELENGTH>::GetPrev(int iItemIdx, int& iPrevItemIdx)
{
     if (iItemIdx < 0 || iItemIdx >= MAXQUEUELENGTH)
     {
          return -1;
     }
     
     if (m_astQueueItems[iItemIdx].m_cUseFlag == ITEM_NOT_USED)
     {
          return -3;
     }

     iPrevItemIdx = m_astQueueItems[iItemIdx].m_iPrevItem;

     return 0;
}

template <int MAXQUEUELENGTH>
int ObjQueue<MAXQUEUELENGTH>::SetNext(int iItemIdx, int iNextItemIdx)
{
     if (iItemIdx < 0 || iItemIdx >= MAXQUEUELENGTH)
     {
          return -1;
     }
     
     m_astQueueItems[iItemIdx].m_iNextItem = iNextItemIdx;

     return 0;
}

template <int MAXQUEUELENGTH>
int ObjQueue<MAXQUEUELENGTH>::SetPrev(int iItemIdx, int iPrevItemIdx)
{
     if (iItemIdx < 0 || iItemIdx >= MAXQUEUELENGTH)
     {
          return -1;
     }
     
     m_astQueueItems[iItemIdx].m_iPrevItem = iPrevItemIdx;

     return 0;
}


#endif