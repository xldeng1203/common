/*
     File creator: neil deng
*/

#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

template<class Type>
class Singleton
{
public:
     virtual ~Singleton()
     {
          m_singleton = NULL;
     }

     static Type& get_singleton()
     {
          if (m_singleton == NULL)
          {
               new Type();
          }
          return *m_singleton;          
     }

     template <typename Type1>
     static Type1& get_singleton()
     {
          if (m_singleton == NULL)
          {
               new Type();
          }
          return *((Type1*)m_singleton);          
     }

     static Type* get_singleton_ptr()
     {
          if (m_singleton == NULL)
          {
               new Type();
          }
          return m_singleton; 
     }

     static void del_singleton()
     {
          delete m_singleton;
          m_singleton = NULL;
     }

protected:
     Singleton()
     {
          assert(m_singleton == NULL);
          m_singleton = static_cast<Type*>(this);
     }

     static Type* m_singleton;
};

template<class Type> Type * Singleton<Type>:: m_singleton = NULL;

#endif