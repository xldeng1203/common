/*******************************************************************
** File Name:       my_array.h
** Copy Right:      neil deng
** File creator:    neil deng
** Date:            
** Version:
** Desc:            
********************************************************************/

#ifndef _MY_ARRAY_H_
#define _MY_ARRAY_H_

template <typename DATA_TYPE, int MAX_NODE_NUM>
class CMyArray
{
public:
     CMyArray();
     ~CMyArray();
public:
     DATA_TYPE& operator [](int iIdx);
     int AddOneElement(DATA_TYPE& rstItem);
     int DelOneElement(int iIdx);
     int Clear();

private:
     int m_iItemNum;
     DATA_TYPE m_astItems[MAX_NODE_NUM];
     const int m_iMaxSize;
};

template <typename DATA_TYPE, int MAX_NODE_NUM>
CMyArray<DATA_TYPE, MAX_NODE_NUM>::CMyArray() : m_iMaxSize(MAX_NODE_NUM)
{
     m_iItemNum = 0;
}

template <typename DATA_TYPE, int MAX_NODE_NUM>
CMyArray<DATA_TYPE, MAX_NODE_NUM>::~CMyArray()
{

}

template <typename DATA_TYPE, int MAX_NODE_NUM>
DATA_TYPE& CMyArray<DATA_TYPE, MAX_NODE_NUM>::operator [](int iIdx)
{
     if (iIdx < 0 || m_iItemNum == 0)
     {
          return m_astItems[0];
     }
     else if (iIdx >= m_iItemNum)
     {
          return m_astItems[m_iItemNum-1];
     }
     
     return m_astItems[iIdx];
}

template <typename DATA_TYPE, int MAX_NODE_NUM>
int CMyArray<DATA_TYPE, MAX_NODE_NUM>::AddOneElement(DATA_TYPE& rstItem)
{
     if (iIdx >= MAX_NODE_NUM)
     {
          return -1;
     }

     m_astItems[m_iItemNum] = rstItem;
     ++m_iItemNum;
     return 0;
}

template <typename DATA_TYPE, int MAX_NODE_NUM>
int CMyArray<DATA_TYPE, MAX_NODE_NUM>::DelOneElement(int iIdx)
{
     if (iIdx < 0 || iIdx >= m_iItemNum)
     {
          return -1;
     }

     --m_iItemNum;
     m_astItems[iIdx] = m_astItems[m_iItemNum];
     return 0;
}

template <typename DATA_TYPE, int MAX_NODE_NUM>
int CMyArray<DATA_TYPE, MAX_NODE_NUM>::Clear()
{
     m_iItemNum = 0;
     return 0;
}


#endif