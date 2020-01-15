#include "object_pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ShareMem* ObjectCreator::pShareMem = NULL;

//extern ShareMem* g_pGeneralMEM;

unsigned char* Object::pObjMem = NULL;

void* Object::operator new( size_t nSize) throw()
{
    if ( !pObjMem )
    {
        return NULL;
    }

    return (void *)pObjMem;
}

void Object::operator delete( void *pMem )
{
    return;
}

int Object::GetObjectID()
{
    return m_iObjectID;
}

size_t ObjectCreator::CalcObjectSize(size_t nObjSize， int iItemCount)
{
    size_t iTempSize = 0;

    iTempSize += sizeof(ObjectCreator);
    iTempSize += iItemCount*sizeof(ObjectWrapper);
    iTempSize += iItemCount*nObjSize;

    return iTempSize;
}

ObjectCreator::ObjectCreator(size_t nObjSize， int iItemCount, Object* (*pfCreateObj)（void *）, bool bInit /* = true*/)
{
    
    /*if ( !pSharedMem)
    {
        return;
    }*/

    m_nObjSize = nObjSize;
    m_iItemCount = iItemCount;

    /*m_pWrappers = (ObjectWrapper *)(pShareMem->AssignMemory(m_iItemCount*sizeof(ObjectWrapper)));
    m_pObjs = (Object*)(pShareMem->AssignMemory(m_iItemCount*m_nObjSize));*/

    m_pWrappers = new ObjectWrapper[m_iItemCount];

    size_t uiObjMem = m_iItemCount * m_nObjSize;
    m_pcObjMem = new char[uiObjMem];
    m_pObjs = (Object*) m_pcObjMem;
    if ( ! m_pWrappers || !m_pObjs )
    {
        return;
    }
    
    m_pfCreateFunc = pfCreateObj;
    if (bInit)
    {
        Initialize();
    }

    MakeObject();
}

ObjectCreator::~ObjectCreator()
{
    if (m_pWrappers != NULL)
    {
        delete[] m_pWrappers;
        m_pWrappers = NULL;
    }

    m_pObjs = NULL;
    if (m_pcObjMem != NULL)
    {
        delete[] m_pcObjMem;
        m_pcObjMem = NULL;
    }
}

int ObjectCreator::Initialize()
{
    m_iIsFormated = 0;
    m_iFreeHead = -1;
    m_iBusyHead = -1;
    m_iBusyCount = 0;

    if ( MakeWrappers())
    {
        return -1;
    }
    
    m_iIsFormated = 1;

    return 0;
}

int ObjectCreator::MakeObject()
{
    Object* pTempObj；
    if ( !m_pfCreateFunc )
    {
        return -1;
    }

    for ( int i = 0; i < m_iItemCount; i++)
    {
        pTempObj = (*m_pfCreateFunc)((void *)((char*)m_pObjs + m_nObjSize*i));
        pTempObj->m_iObjectID = i;
        m_pWrappers[i].WrapObj(pTempObj);
    }
    
    return 0;
}

int ObjectCreateor::MakeWrappers()
{
    m_iFreeHead = 0;
    m_iBusyHead = -1;

    m_pWrappers[m_iFreeHead].Initialize();
    m_pWrappers[m_iFreeHead].m_iPrevIdx = -1;

    for (int i = 1; i < m_iItemCount; i++)
    {
        m_pWrappers[i].Initialize();
        m_pWrappers[i-1].m_iNextIdx = i;
        m_pWrappers[i].m_iPrevIdx = i - 1;
    }
    
    m_pWrappers[m_iItemCount-1].m_iNextId = -1;
    return 0;
}

int ObjectCreateor::BusyCount()
{
    return m_iBusyCount;
}

int ObjectCreateor::FreeCount()
{
    return m_iItemCount - m_iBusyCount;
}

int ObjectCreateor::BusyHead()
{
    return m_iBusyHead;
}

int ObjectCreateor::FreeHead()
{
    return m_iFreeHead;
}

ObjectWrapper* ObjectCreator::GetWrapper(int iIdx)
{
    if ( iIdx < 0 || iIdx >= m_iItemCount )
    {
        return NULL;
    }

    if (!m_pWrappers[iIdx].m_iIsBusy)
    {
        return NULL;
    }
    
    return &m_pWrappers[iIdx];
}

Object* ObjectCreator::GetObject(int iIdx)
{
    if ( iIdx < 0 || iIdx >= m_iItemCount )
    {
        return NULL;
    }

    if (!m_pWrappers[iIdx].m_iIsBusy)
    {
        return NULL;
    }

     return m_pWrappers[iIdx].GetWrapperObj();
}

int ObjectCreator::CreateObject()
{
    int iTempIdx = 0;
    if (m_iBusyCount >= m_iItemCount)
    {
        return -1;
    }

    iTempIdx = m_iFreeHead;
    m_iFreeHead = m_pWrappers[iTempIdx].m_iNextIdx;
    if (m_iFreeHead > 0)
    {
        m_pWrappers[m_iFreeHead].m_iPrevIdx = -1;
    }
    
    m_pWrappers[iTempIdx].m_iIsBusy = 1;
    m_pWrappers[iTempIdx].m_iNextIdx = m_iBusyHead;
    m_pWrappers[iTempIdx].m_iPrevIdx = -1;

    if (m_iBusyHead >= 0)
    {
        m_pWrappers[m_iBusyHead].m_iPrevIdx = iTempIdx;
    }
    
    m_iBusyHead = iTempIdx;
    /* m_pWrappers[iTempIdx].GetWrappedObj()->Initialize();*/
    m_iBusyCount++;

    return iTempIdx;
}

int ObjectCreator::DestoryObject(int iIdx)
{
    if (iIdx >= m_iItemCount || iIdx < 0 || m_iItemCount <= 0)
    {
        return -1;
    }

    if ( !m_pWrappers[iIdx].m_iIsBusy)
    {
        return -2;
    }

    if (iIdx == m_iBusyHead)
    {
        m_iBusyHead = m_pWrappers[iIdx].m_iNextIdx;
        if (m_iBusyHead >= 0)
        {
            m_pWrappers[m_iBusyHead].m_iPrevIdx = -1;
        }       
    }
    
    if (m_pWrappers[iIdx].m_iNextIdx >= 0)
    {
        m_pWrappers[m_pWrappers[iIdx].m_iNextIdx].m_iPrevIdx = m_pWrappers[iIdx].m_iPrevIdx;
    }
    
    if (m_pWrappers[iIdx].m_iPrevIdx >= 0)
    {
        m_pWrappers[m_pWrappers[iIdx].m_iPrevIdx].m_iNextIdx = m_pWrappers[iIdx].m_iNextIdx;
    } 

    //挂入空闲队列待分配
    m_pWrappers[iIdx].m_iIsBusy = 0;
    m_pWrappers[iIdx].m_iPrevIdx = -1;
    m_pWrappers[iIdx].m_iNextIdx = m_iFreeHead;
    if (m_iFreeHead >= 0)
    {
        m_pWrappers[m_iFreeHead].m_iPrevIdx = iIdx;
    }
    m_iFreeHead = iIdx;
    m_iBusyCount--;

    return iIdx;
}

ObjectWrapper::ObjectWrapper()
{
    Initialize();
}

ObjectWrapper::~ObjectWrapper()
{
}

void ObjectWrapper::Initialize()
{
    m_iNextIdx = -1;
    m_iPrevIdx = -1;
    m_iIsBusy = 0;
    m_pAttachedObj = NULL;
}

int ObjectWrapper::GetWrapperObj()
{
    return m_pAttachedObj;
}

int ObjectWrapper::WrapObj(Object* pObj)
{
    if (!pObj)
    {
        return -1;
    }
    
    m_pAttachedObj = pObj;
    return 0;
}