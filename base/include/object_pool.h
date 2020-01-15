/*
     File creator: neil deng
*/

#ifndef _OBJECT_POOL_H_
#define _OBJECT_POOL_H_

class ObjectWrapper;
class ObjectCreator;
class Object;

class ObjectCreator
{
     ObjectCreator(size_t nObjSize， int iItemCount, Object* (*pfCreateObj)（void *）, bool bInit = true);    //是对象创建函数
     ~ObjectCreator();

     int Initalize();
     int MakeObject();
     int MakeWrappers();
     int CreateObject();
     int DestoryObject(int iIdx);

     int BusyHead();
     int FreeCount();
     int BusyCount();
     int FreeCount();
     
     Object* GetObject(int iIdx);
     ObjectWrapper* GetWrapper(int iIdx);

     static ShareMem* pShareMem;
     static size_t CalcObjectSize(size_t nObjSize， int iItemCount);

private:
     int       m_iIsFormated;
     size_t    m_nObjSize;
     ObjectWrapper * m_pWrappers;
     Object*   m_pObjs;
     char*     m_pcObjMem;
     int       m_iItemCount;
     int       m_iFreeHead;
     int       m_iBusyHead;
     int       m_iBusyCount;
     Object*   (*m_pfCreateFunc)(void*);
};

class ObjectWrapper
{
public:
     ObjectWrapper();
     ~ObjectWrapper();

     Object* GetWrapperObj();
     int WrapObj(Object* pObj);
     void Initialize();
     int GetNextIdx();

     int m_iIsBusy;
     int m_iNextIdx;
     int m_iPrevIdx;

private:
     Object    *m_pAttachedObj;
};

class Object
{
public:
     Object();
     virtual ~Object();
     virtual int Initalize() = 0;
     int GetObjectID();

     static void* operator new( size_t nSize ) throw();
     static void operator delete( void *pMem);
     static unsigned char *pObjMem;

private:
     int m_iObjectID;
     friend int ObjectCreator::MakeObject();
};


#endif