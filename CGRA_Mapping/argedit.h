/*------------------------------------------------------------------
 * argedit.h
 * 简单的支持图编辑操作的ARG输入接口定义
 *
 * Author:Li Xiaoquan
 * 2016/3/9
 *-----------------------------------------------------------------*/



#ifndef ARGEDIT_H
#define ARGEDIT_H

#include "argraph.h"


/*---------------------------------------------------------
 * Class ARGEdit
 * 一个简单的ARGLoader，支持图编辑操作。
 * Note: ARGEdit不规定属性的存储，这些需要后面使用时额外处理
 -------------------------------------------------------*/
class ARGEdit: public ARGLoader
  { 
    public:

      ARGEdit();
      ARGEdit(ARGraph_impl &g);
      ARGEdit(ARGLoader &g);
      ~ARGEdit();

      /* 重定义ARGLoader的方法 */
      virtual int NodeCount();
      virtual void *GetNodeAttr(node_id node);
      virtual int OutEdgeCount(node_id node);
      virtual node_id GetOutEdge(node_id node, int i, void **pattr);

      /* 图编辑操作 */
      node_id InsertNode(void *attr);
      void InsertEdge(node_id n1, node_id n2, void *attr);
      void DeleteNode(node_id n);
      void DeleteEdge(node_id n1, node_id n2);

    protected:
      int count;

      struct eNode
        { node_id from;
          node_id to;
          int pos;
          void *attr;
          eNode *next;
        };

      struct nNode 
        { node_id id;
          int count;
          void *attr;
          nNode *next;
          eNode *edges;
        };

      nNode *nodes;
      nNode *lastNode;
      eNode *lastEdge;

      virtual void destroyNodeAttr(void *) {};
      virtual void destroyEdgeAttr(void *) {};
  };

#endif
