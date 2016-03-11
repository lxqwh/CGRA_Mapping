/*----------------------------------------------------
 * sortnodes.cc
 * 根据前驱后继的频率对结点进行排序
 ---------------------------------------------------*/

#include <stdlib.h>
#include "sortnodes.h"

#if 0
// 测试main函数
#include "argedit.h"
#include <stdio.h>
int main(int argc, char *argv[])
  { int n=argc>1? atoi(argv[1]): 10;
    if (n<1) n=10;
    ARGEdit ed;
    int i,j;
    for(i=0; i<n; i++)
      ed.InsertNode(NULL);
    for(i=0; i<n; i++)
      { for(j=1; j<i-(3-i)%4; j++)
          { ed.InsertEdge(i, (i+j)%n, NULL);
	    printf("(%d-%d) ", i, (i+j)%n);
	  }
      }
	printf("\n");
    Graph g(&ed);
    node_id *s=SortNodesByFrequency(&g);
    for(i=0; i<n; i++)
      printf("%d ", s[i]);
    printf("\n");
    return 0;
  }
#endif

typedef int (*compare_fn)(const void *, const void *);

struct NodeInfo
  { node_id id;
    node_id in;
    node_id out;
  };

static int nodeInfoComp1(NodeInfo *a, NodeInfo *b);
static int nodeInfoComp2(NodeInfo *a, NodeInfo *b);

/*----------------------------------------------------
 * 对图的结点进行排序，返回一个正序的堆（vector）
 * 排序策略中需要考虑的因素:
 *    1 - 相同出入度的结点
 *    2 - 结点的权值
 * 排在第一的结点是最孤立的，匹配会从这个点开始
 *--------------------------------------------------*/
node_id* SortNodesByFrequency(Graph *g)
  { int n=g->NodeCount();
    NodeInfo *vect=new NodeInfo[n];
    int i;
    for(i=0; i<n; i++)
      { vect[i].id=i;
        vect[i].in=g->InEdgeCount(i);
        vect[i].out=g->OutEdgeCount(i);
      }
    
    //for(i=0; i<n; i++)
    //  printf("<%d/%d/%d> ", vect[i].id, vect[i].in, vect[i].out);
    //printf("\n");

    qsort(vect, n, sizeof(vect[0]), (compare_fn)nodeInfoComp1);

    //for(i=0; i<n; i++)
    //  printf("<%d/%d/%d> ", vect[i].id, vect[i].in, vect[i].out);
    //printf("\n");

    int run=1;

    for(i=0; i<n; i+=run)
      { for(run=1; i+run<n && 
                   vect[i+run].in==vect[i].in && 
		   vect[i+run].out==vect[i].out;
            run++)
	  ;
	int j;
	for(j=0; j<run; j++)
	  { vect[i+j].in += vect[i+j].out;
	    vect[i+j].out=run;
	  }
       }
	

    //for(i=0; i<n; i++)
    //  printf("<%d/%d/%d> ", vect[i].id, vect[i].in, vect[i].out);
    //printf("\n");

    qsort(vect, n, sizeof(vect[0]), (compare_fn)nodeInfoComp2);

    //for(i=0; i<n; i++)
    //  printf("<%d/%d/%d> ", vect[i].id, vect[i].in, vect[i].out);
    //printf("\n");

    node_id *nodes=new node_id[n];
    for(i=0; i<n; i++)
      nodes[i]=vect[i].id;
    
    delete[] vect;

    return nodes;
  }

   




/**
 * 根据出入度排序
 */
static int nodeInfoComp1(NodeInfo *a, NodeInfo *b)
  { if (a->out < b->out)
      return -1;
    else if (a->out > b->out)
      return +1;
    else if (a->in < b->in)
      return -1;
    else if (a->in > b->in)
      return +1;
    else 
      return 0;
  }

/**
 * 根据频率和权值排序.
 * out域存放频率, in域存放权值.
 */
static int nodeInfoComp2(NodeInfo *a, NodeInfo *b)
  { if (a->in==0 && b->in !=0)
      return +1;
    else if (a->in!=0 && b->in==0)
      return -1;
    else if (a->out < b->out)
      return -1;
    else if (a->out > b->out)
      return +1;
    else if (a->in < b->in)
      return -1;
    else if (a->in > b->in)
      return +1;
    else 
      return 0;
  }
