/*------------------------------------------------------------------
 * argedit.h
 * �򵥵�֧��ͼ�༭������ARG����ӿڶ���
 *
 * Author:Li Xiaoquan
 * 2016/3/9
 *-----------------------------------------------------------------*/



#ifndef ARGEDIT_H
#define ARGEDIT_H

#include "argraph.h"


/*---------------------------------------------------------
 * Class ARGEdit
 * һ���򵥵�ARGLoader��֧��ͼ�༭������
 * Note: ARGEdit���涨���ԵĴ洢����Щ��Ҫ����ʹ��ʱ���⴦��
 -------------------------------------------------------*/
class ARGEdit: public ARGLoader
  { 
    public:

      ARGEdit();
      ARGEdit(ARGraph_impl &g);
      ARGEdit(ARGLoader &g);
      ~ARGEdit();

      /* �ض���ARGLoader�ķ��� */
      virtual int NodeCount();
      virtual void *GetNodeAttr(node_id node);
      virtual int OutEdgeCount(node_id node);
      virtual node_id GetOutEdge(node_id node, int i, void **pattr);

      /* ͼ�༭���� */
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
