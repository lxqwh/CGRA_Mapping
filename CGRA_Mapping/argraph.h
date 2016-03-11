/*------------------------------------------------------------------
 * argraph.h
 * argraph.cc的接口
 * 属性图类的接口定义
 *-----------------------------------------------------------------*/


#ifndef ARGRAPH_H
#define ARGRAPH_H

#include <assert.h>
#include <stddef.h>

#ifdef NEED_BOOL
#ifndef BOOL_DEFINED
#define BOOL_DEFINED
typedef int bool;
const false=0;
const true=1
#endif
#endif

#ifndef BYTE_DEFINED
#define BYTE_DEFINED
typedef unsigned char byte;
#endif

typedef unsigned short node_id;
const node_id NULL_NODE=0xFFFF;
class ARGLoader
  { public:
      virtual ~ARGLoader() {}


      virtual int NodeCount() = 0;
      virtual void *GetNodeAttr(node_id node) = 0;
      virtual int OutEdgeCount(node_id node) = 0;
      virtual node_id GetOutEdge(node_id node, int i, void **pattr)=0;
  };


class AttrDestroyer
  { public:
      virtual ~AttrDestroyer() {};
      virtual void destroy(void *attr)=0;
  };
class AttrComparator
  { public:
      virtual ~AttrComparator() {};
      virtual bool compatible(void *attr1, void *attr2)=0;
  };


class ARGraph_impl
  { public:
      typedef void *param_type;
      typedef void (*edge_visitor)(ARGraph_impl *g, 
                                   node_id n1, node_id n2, void *attr,
                                   param_type param);
      typedef void (*node_destroy_fn)(void *);
      typedef void (*edge_destroy_fn)(void *);

      typedef bool (*node_compat_fn)(void *, void *);
      typedef bool (*edge_compat_fn)(void *, void *);



    private:
      typedef short count_type;

      int n;              /* nodes 个数 */
      void* *attr;        /* node 属性  */
      count_type  *in_count;  /*每个节点的入边*/
      node_id **in;       /* 每个节点入边的起点 */
      void* **in_attr;    /* 入边属性*/
      count_type  *out_count; /* 每个节点的出边 */
      node_id **out;      /* 出边的终点*/
      void* **out_attr;   /* 出边的属性 */

      AttrDestroyer *node_destroyer;  // 节点属性清除
      AttrDestroyer *edge_destroyer;  // 边属性清除
      AttrComparator *node_comparator; // 节点属性比较
      AttrComparator *edge_comparator; // 边属性比较




    public: 
      void SetNodeDestroyer(AttrDestroyer *);
      void SetEdgeDestroyer(AttrDestroyer *);
      void SetNodeDestroy(node_destroy_fn fn); 
      void SetEdgeDestroy(edge_destroy_fn fn); 

      void SetNodeComparator(AttrComparator *);
      void SetEdgeComparator(AttrComparator *);
      void SetNodeCompat(node_compat_fn); 
      void SetEdgeCompat(edge_compat_fn); 

    public:
      ARGraph_impl(ARGLoader *loader);
      virtual ~ARGraph_impl();


      int NodeCount();

      void *GetNodeAttr(node_id i);
      void SetNodeAttr(node_id i, void *attr, bool destroyOld=false);

      bool HasEdge(node_id n1, node_id n2);
      bool HasEdge(node_id n1, node_id n2, void **pattr);
      void *GetEdgeAttr(node_id n1, node_id n2);
      void SetEdgeAttr(node_id n1, node_id n2, void *attr, 
                       bool destroyOld=false);

      int InEdgeCount(node_id node);
      int OutEdgeCount(node_id node); 
      int EdgeCount(node_id node); 
      node_id GetInEdge(node_id node, int i);
      node_id GetInEdge(node_id node, int i, void **pattr);
      node_id GetOutEdge(node_id node, int i);
      node_id GetOutEdge(node_id node, int i, void **pattr);


      void VisitInEdges(node_id node, edge_visitor vis, param_type param);
      void VisitOutEdges(node_id node, edge_visitor vis, param_type param);
      void VisitEdges(node_id node, edge_visitor vis, param_type param);

      virtual bool CompatibleNode(void *attr1, void *attr2);
      virtual bool CompatibleEdge(void *attr1, void *attr2);

    protected:
      virtual void DestroyNode(void *attr);
      virtual void DestroyEdge(void *attr);
  };


typedef ARGraph_impl Graph;



template <class Node, class Edge>
class ARGraph: public ARGraph_impl
  { public:
      typedef Node node_type;
      typedef Edge edge_type;

      typedef void *param_type;
      typedef void (*edge_visitor)(ARGraph *g, 
                                   node_id n1, node_id n2, Edge *attr,
                                   param_type param);
      typedef void (*node_destroy_fn)(Node *);
      typedef void (*edge_destroy_fn)(Edge *);

      typedef bool (*node_compat_fn)(Node *, Node *);
      typedef bool (*edge_compat_fn)(Edge *, Edge *);

      typedef ARGraph_impl impl;



      ARGraph(ARGLoader *loader) : impl(loader) {}

      void SetNodeDestroy(node_destroy_fn fn)
          { ARGraph_impl::SetNodeDestroy((ARGraph_impl::node_destroy_fn) fn); }
      void SetEdgeDestroy(edge_destroy_fn fn)
          { ARGraph_impl::SetEdgeDestroy((ARGraph_impl::edge_destroy_fn) fn); }

      void SetNodeCompat(node_compat_fn fn)
          { ARGraph_impl::SetNodeCompat((ARGraph_impl::node_compat_fn) fn); }
      void SetEdgeCompat(edge_compat_fn fn)
          { ARGraph_impl::SetEdgeCompat((ARGraph_impl::edge_compat_fn) fn); }


      bool HasEdge(node_id n1, node_id n2, Edge **pattr)
          { return ARGraph_impl::HasEdge(n1, n2, (void**)pattr); }

      Node *GetNodeAttr(node_id i)
          { return (Node*)ARGraph_impl::GetNodeAttr(i); }

      Edge *GetEdgeAttr(node_id n1, node_id n2)
                 { return (Edge*)ARGraph_impl::GetEdgeAttr(n1,n2); }

      node_id GetInEdge(node_id node, int i, Edge **pattr)
                 { return ARGraph_impl::GetInEdge(node, i, (void**)pattr); }
      node_id GetOutEdge(node_id node, int i, Edge **pattr)
                 { return ARGraph_impl::GetOutEdge(node, i, (void**)pattr); }


      void VisitInEdges(node_id node, edge_visitor vis, param_type param)
         { ARGraph_impl::VisitInEdges(node, (ARGraph_impl::edge_visitor)vis, 
                                      param); }
      void VisitOutEdges(node_id node, edge_visitor vis, param_type param)
         { ARGraph_impl::VisitOutEdges(node, (ARGraph_impl::edge_visitor)vis, 
                                       param); }
      void VisitEdges(node_id node, edge_visitor vis, param_type param)
         { ARGraph_impl::VisitEdges(node, (ARGraph_impl::edge_visitor)vis, 
                                    param); }


      bool CompatibleNode(Node *attr1, Node *attr2)
         { return ARGraph_impl::CompatibleNode(attr1, attr2); }
      bool CompatibleEdge(Edge *attr1, Edge *attr2)
         { return ARGraph_impl::CompatibleEdge(attr1, attr2); }
  };



class FunctionAttrDestroyer: public AttrDestroyer
  { private:
      void (*func)(void *);
    public:
      FunctionAttrDestroyer(void (*fn)(void *));
      virtual void destroy(void *attr);
  };

class FunctionAttrComparator: public AttrComparator
  { private:
      bool (*func)(void *, void *);
    public:
      FunctionAttrComparator(bool (*fn)(void *, void *));
      virtual bool compatible(void *attr1, void *attr2);
  };


inline int ARGraph_impl::NodeCount() 
  { return n;
  } 


inline void * ARGraph_impl::GetNodeAttr(node_id i) 
  { assert(i<n);
    return attr[i];
  }

inline bool ARGraph_impl::HasEdge(node_id n1, node_id n2)
    { return HasEdge(n1, n2, NULL);
    }


inline void* ARGraph_impl::GetEdgeAttr(node_id n1, node_id n2)
    { void *attr;
      if (HasEdge(n1, n2, &attr))
         return attr;
      else
         return NULL;
    }



inline int ARGraph_impl::InEdgeCount(node_id node) 
  { assert(node<n); 
    return in_count[node]; 
  }



inline int ARGraph_impl::OutEdgeCount(node_id node) 
  { assert(node<n); 
    return out_count[node]; 
  }


inline int ARGraph_impl::EdgeCount(node_id node) 
  { assert(node<n); 
    return in_count[node]+out_count[node]; 
  }


inline node_id ARGraph_impl::GetInEdge(node_id node, int i)
  { assert(node<n);
    assert(i<in_count[node]);
    return in[node][i];
  }


inline node_id ARGraph_impl::GetInEdge(node_id node, int i, 
                                       void **pattr)
  { assert(node<n);
    assert(i<in_count[node]);
    *pattr = in_attr[node][i];
    return in[node][i];
  }



inline node_id ARGraph_impl::GetOutEdge(node_id node, int i)
  { assert(node<n);
    assert(i<out_count[node]);
    return out[node][i];
  }


inline node_id ARGraph_impl::GetOutEdge(node_id node, int i, 
                                        void **pattr)
  { assert(node<n);
    assert(i<out_count[node]);
    *pattr = out_attr[node][i];
    return out[node][i];
  }


inline bool ARGraph_impl::CompatibleNode(void *attr1, void *attr2)
  { if (node_comparator==NULL)
      return true;
    else
      return node_comparator->compatible(attr1, attr2);
  }



inline bool ARGraph_impl::CompatibleEdge(void *attr1, void *attr2)
  { if (edge_comparator==NULL)
      return true;
    else
      return edge_comparator->compatible(attr1, attr2);
  }


#endif


