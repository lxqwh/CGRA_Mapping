/*-----------------------------------------------------
 * allocpool.h
 * 对象池类
 ----------------------------------------------------*/

/*----------------------------------------------------------------------
 * 
 * 在堆上进行有效地分配同类对象。这些对象会被同时地进行析构，这样的分配
 * 方式比单独分离的分配在时间和空间上都具有更高的效率。
 * “chunks”表示多个已分配的目标集合组，分配池保存有这些chunks的链表，
 * 在对象进行析构时一下子删除所有的chunks。
 * 共有两个类: Allocator是一个借口类，与具体的策略分离
 *             AllocationPool 正在的对象池类。 
 ---------------------------------------------------------------------*/

#ifndef ALLOCPOOL_H
#define ALLOCPOOL_H


#include <stddef.h>
#include "error.h"

/*-----------------------------------------------------------
 * Allocator类声明，抽象
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
 * NewAllocator声明,简单实用new来分配对象
 ----------------------------------------------------------*/
template <class T>
class NewAllocator: public Allocator<T>
  { protected:
      virtual T* alloc() { return new T; }
  };

/*-----------------------------------------------------------
 * NullAllocator声明，总是返回NULL指针
 ----------------------------------------------------------*/
template <class T>
class NullAllocator: public Allocator<T>
  { public:
      virtual T *Allocate() { return NULL; }
	protected:
	  virtual T *alloc() { return NULL; }
  };


/*-----------------------------------------------------------
 * AllocationPool类声明
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
 * AllocationPool类的内联函数
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
