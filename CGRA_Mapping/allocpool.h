/*-----------------------------------------------------
 * allocpool.h
 * �������
 ----------------------------------------------------*/

/*----------------------------------------------------------------------
 * 
 * �ڶ��Ͻ�����Ч�ط���ͬ�������Щ����ᱻͬʱ�ؽ��������������ķ���
 * ��ʽ�ȵ�������ķ�����ʱ��Ϳռ��϶����и��ߵ�Ч�ʡ�
 * ��chunks����ʾ����ѷ����Ŀ�꼯���飬����ر�������Щchunks������
 * �ڶ����������ʱһ����ɾ�����е�chunks��
 * ����������: Allocator��һ������࣬�����Ĳ��Է���
 *             AllocationPool ���ڵĶ�����ࡣ 
 ---------------------------------------------------------------------*/

#ifndef ALLOCPOOL_H
#define ALLOCPOOL_H


#include <stddef.h>
#include "error.h"

/*-----------------------------------------------------------
 * Allocator������������
 *---------------------------------------------------------*/

template <class T>
class Allocator
  { protected:
      virtual T *alloc() = 0;
    public:
      virtual ~Allocator() {}
      virtual T *Allocate() 
          { T*p=alloc();
            if (p==NULL)
              error("Out of memory");
            return p;
          }
  };


/*-----------------------------------------------------------
 * NewAllocator����,��ʵ��new���������
 ----------------------------------------------------------*/
template <class T>
class NewAllocator: public Allocator<T>
  { protected:
      virtual T* alloc() { return new T; }
  };

/*-----------------------------------------------------------
 * NullAllocator���������Ƿ���NULLָ��
 ----------------------------------------------------------*/
template <class T>
class NullAllocator: public Allocator<T>
  { public:
      virtual T *Allocate() { return NULL; }
	protected:
	  virtual T *alloc() { return NULL; }
  };


/*-----------------------------------------------------------
 * AllocationPool������
 *---------------------------------------------------------*/

template <class T, int CHUNK_SIZE>
class AllocationPool: public Allocator<T>
  { private:
      struct chunk
        { chunk *link;
          T content[CHUNK_SIZE];
        };
      chunk *chunkList;
      int rest;
      void grow();
	protected:
      virtual T *alloc();
    public:
      AllocationPool();
      ~AllocationPool();
  };



/*----------------------------------------------------
 * AllocationPool�����������
 ---------------------------------------------------*/
template <class T, int CHUNK_SIZE>
AllocationPool<T, CHUNK_SIZE>::AllocationPool() 
  { chunkList=0; 
    rest=0; 
  }

template <class T, int CHUNK_SIZE>
AllocationPool<T, CHUNK_SIZE>::~AllocationPool() 
  { chunk *p=chunkList;
    while (p!=0)
      { chunkList=p->link;
        delete p;
        p=chunkList;
      }
  }

template <class T, int CHUNK_SIZE>
T * AllocationPool<T, CHUNK_SIZE>::alloc() 
  { if (rest==0)
      grow();
    if (rest>0)
      return chunkList->content + (--rest);
    else
      return 0;
  }

template <class T, int CHUNK_SIZE>
void AllocationPool<T, CHUNK_SIZE>::grow() 
  { chunk *p=new chunk;
    if (p!=0)
      { p->link=chunkList;
        chunkList=p;
        rest=CHUNK_SIZE;
      }
  }

#endif
