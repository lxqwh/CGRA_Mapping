/*------------------------------------------------------------------
 * vf2_state_monomorphism.cc
 * VF2MonoState类实现
 *-----------------------------------------------------------------*/

#include <stddef.h>

#include "vf2_state_monomorphism.h"
#include "sort_nodes.h"

#include "error.h"


VF2MonoState::VF2MonoState(Graph *ag1, Graph *ag2, bool sortNodes)
  { g1=ag1;
    g2=ag2;
    n1=g1->NodeCount();
    n2=g2->NodeCount();

    if (sortNodes)
      order = SortNodesByFrequency(ag1);
    else
      order = NULL;

    core_len=orig_core_len=0;
    t1both_len=t1in_len=t1out_len=0;
    t2both_len=t2in_len=t2out_len=0;

	added_node1=NULL_NODE;

    core_1=new node_id[n1];
    core_2=new node_id[n2];
    in_1=new node_id[n1];
    in_2=new node_id[n2];
    out_1=new node_id[n1];
    out_2=new node_id[n2];
	share_count = new long;
    if (!core_1 || !core_2 || !in_1 || !in_2 
	    || !out_1 || !out_2 || !share_count)
      error("Out of memory");

    int i;
    for(i=0; i<n1; i++)
      { 
        core_1[i]=NULL_NODE;
		in_1[i]=0;
		out_1[i]=0;
      }
    for(i=0; i<n2; i++)
      { 
        core_2[i]=NULL_NODE;
		in_2[i]=0;
		out_2[i]=0;
      }

	*share_count = 1;
  }


/*----------------------------------------------------------
 * VF2MonoState::VF2MonoState(state)
 * Copy 构造函数. 
 ---------------------------------------------------------*/
VF2MonoState::VF2MonoState(const VF2MonoState &state)
  { g1=state.g1;
    g2=state.g2;
    n1=state.n1;
    n2=state.n2;

    order=state.order;

    core_len=orig_core_len=state.core_len;
    t1in_len=state.t1in_len;
    t1out_len=state.t1out_len;
    t1both_len=state.t1both_len;
    t2in_len=state.t2in_len;
    t2out_len=state.t2out_len;
    t2both_len=state.t2both_len;

	added_node1=NULL_NODE;

    core_1=state.core_1;
    core_2=state.core_2;
    in_1=state.in_1;
    in_2=state.in_2;
    out_1=state.out_1;
    out_2=state.out_2;
    share_count=state.share_count;

	++ *share_count;

  }


/*---------------------------------------------------------------
 * VF2MonoState::~VF2MonoState()
 * 析构函数.
 --------------------------------------------------------------*/
VF2MonoState::~VF2MonoState() 
  { if (-- *share_count == 0)
    { delete [] core_1;
      delete [] core_2;
      delete [] in_1;
      delete [] out_1;
      delete [] in_2;
      delete [] out_2;
      delete share_count;
      delete [] order;
	}
  }


bool VF2MonoState::NextPair(node_id *pn1, node_id *pn2,
              node_id prev_n1, node_id prev_n2)
  { 
    if (prev_n1==NULL_NODE)
      prev_n1=0;
    if (prev_n2==NULL_NODE)
      prev_n2=0;
    else
      prev_n2++;

	if (t1both_len>core_len && t2both_len>core_len)
	  { while (prev_n1<n1 &&
           (core_1[prev_n1]!=NULL_NODE || out_1[prev_n1]==0
		            || in_1[prev_n1]==0) )
          { prev_n1++;    
            prev_n2=0;
          }
	  }
	else if (t1out_len>core_len && t2out_len>core_len)
	  { while (prev_n1<n1 &&
           (core_1[prev_n1]!=NULL_NODE || out_1[prev_n1]==0) )
          { prev_n1++;    
            prev_n2=0;
          }
	  }
    else if (t1in_len>core_len && t2in_len>core_len)
	  { while (prev_n1<n1 &&
           (core_1[prev_n1]!=NULL_NODE || in_1[prev_n1]==0) )
          { prev_n1++;    
            prev_n2=0;
          }
	  }
	else if (prev_n1==0 && order!=NULL)
	  { int i=0;
	    while (i<n1 && core_1[prev_n1=order[i]] != NULL_NODE)
	      i++;
	    if (i==n1)
	      prev_n1=n1;
	  }
	else
	  { while (prev_n1<n1 && core_1[prev_n1]!=NULL_NODE )
          { prev_n1++;    
            prev_n2=0;
          }
	  }
	  


	if (t1both_len>core_len && t2both_len>core_len)
	  { while (prev_n2<n2 &&
           (core_2[prev_n2]!=NULL_NODE || out_2[prev_n2]==0
		            || in_2[prev_n2]==0) )
          { prev_n2++;    
          }
	  }
	else if (t1out_len>core_len && t2out_len>core_len)
	  { while (prev_n2<n2 &&
           (core_2[prev_n2]!=NULL_NODE || out_2[prev_n2]==0) )
          { prev_n2++;    
          }
	  }
    else if (t1in_len>core_len && t2in_len>core_len)
	  { while (prev_n2<n2 &&
           (core_2[prev_n2]!=NULL_NODE || in_2[prev_n2]==0) )
          { prev_n2++;    
          }
	  }
	else
	  { while (prev_n2<n2 && core_2[prev_n2]!=NULL_NODE )
          { prev_n2++;    
          }
	  }
	  

    if (prev_n1<n1 && prev_n2<n2)
          { *pn1=prev_n1;
            *pn2=prev_n2;
            return true;
          }

    return false;
  }




bool VF2MonoState::IsFeasiblePair(node_id node1, node_id node2)
  { assert(node1<n1);
    assert(node2<n2);
    assert(core_1[node1]==NULL_NODE);
    assert(core_2[node2]==NULL_NODE);

    if (!g1->CompatibleNode(g1->GetNodeAttr(node1), g2->GetNodeAttr(node2)))
      return false;

    int i, other1, other2;
    void *attr1;
    int termout1=0, termout2=0, termin1=0, termin2=0, new1=0, new2=0;

    // 检查结node1的出边
    for(i=0; i<g1->OutEdgeCount(node1); i++)
      { other1=g1->GetOutEdge(node1, i, &attr1);
        if (core_1[other1] != NULL_NODE)
          { other2=core_1[other1];
            if (!g2->HasEdge(node2, other2) ||
                !g1->CompatibleEdge(attr1, g2->GetEdgeAttr(node2, other2)))
              return false;
          }
        else 
          { if (in_1[other1])
              termin1++;
            if (out_1[other1])
              termout1++;
            if (!in_1[other1] && !out_1[other1])
              new1++;
          }
      }

    // 检查node1的入边
    for(i=0; i<g1->InEdgeCount(node1); i++)
      { other1=g1->GetInEdge(node1, i, &attr1);
        if (core_1[other1]!=NULL_NODE)
          { other2=core_1[other1];
            if (!g2->HasEdge(other2, node2) ||
                !g1->CompatibleEdge(attr1, g2->GetEdgeAttr(other2, node2)))
              return false;
          }
        else 
          { if (in_1[other1])
              termin1++;
            if (out_1[other1])
              termout1++;
            if (!in_1[other1] && !out_1[other1])
              new1++;
          }
      }


    // 检查node2的出边
    for(i=0; i<g2->OutEdgeCount(node2); i++)
      { other2=g2->GetOutEdge(node2, i);
        if (core_2[other2]!=NULL_NODE)
          { /* Do nothing */
          }
        else 
          { if (in_2[other2])
              termin2++;
            if (out_2[other2])
              termout2++;
            if (!in_2[other2] && !out_2[other2])
              new2++;
          }
      }

    // 检查node2的入边
    for(i=0; i<g2->InEdgeCount(node2); i++)
      { other2=g2->GetInEdge(node2, i);
        if (core_2[other2] != NULL_NODE)
          { /* 直接略过 */
          }
        else 
          { if (in_2[other2])
              termin2++;
            if (out_2[other2])
              termout2++;
            if (!in_2[other2] && !out_2[other2])
              new2++;
          }
      }

    return termin1<=termin2 && termout1<=termout2
	      && (termin1+termout1+new1)<=(termin2+termout2+new2);
  }



void VF2MonoState::AddPair(node_id node1, node_id node2)
  { assert(node1<n1);
    assert(node2<n2);
    assert(core_len<n1);
    assert(core_len<n2);

    core_len++;
	added_node1=node1;

	if (!in_1[node1])
	  { in_1[node1]=core_len;
	    t1in_len++;
		if (out_1[node1])
		  t1both_len++;
	  }
	if (!out_1[node1])
	  { out_1[node1]=core_len;
	    t1out_len++;
		if (in_1[node1])
		  t1both_len++;
	  }

	if (!in_2[node2])
	  { in_2[node2]=core_len;
	    t2in_len++;
		if (out_2[node2])
		  t2both_len++;
	  }
	if (!out_2[node2])
	  { out_2[node2]=core_len;
	    t2out_len++;
		if (in_2[node2])
		  t2both_len++;
	  }

    core_1[node1]=node2;
    core_2[node2]=node1;


    int i, other;
    for(i=0; i<g1->InEdgeCount(node1); i++)
      { other=g1->GetInEdge(node1, i);
        if (!in_1[other])
          { in_1[other]=core_len;
            t1in_len++;
		    if (out_1[other])
		      t1both_len++;
          }
      }

    for(i=0; i<g1->OutEdgeCount(node1); i++)
      { other=g1->GetOutEdge(node1, i);
        if (!out_1[other])
          { out_1[other]=core_len;
            t1out_len++;
		    if (in_1[other])
		      t1both_len++;
          }
      }
    
    for(i=0; i<g2->InEdgeCount(node2); i++)
      { other=g2->GetInEdge(node2, i);
        if (!in_2[other])
          { in_2[other]=core_len;
            t2in_len++;
		    if (out_2[other])
		      t2both_len++;
          }
      }

    for(i=0; i<g2->OutEdgeCount(node2); i++)
      { other=g2->GetOutEdge(node2, i);
        if (!out_2[other])
          { out_2[other]=core_len;
            t2out_len++;
		    if (in_2[other])
		      t2both_len++;
          }
      }

  }



void VF2MonoState::GetCoreSet(node_id c1[], node_id c2[])
  { int i,j;
    for (i=0,j=0; i<n1; i++)
      if (core_1[i] != NULL_NODE)
        { c1[j]=i;
          c2[j]=core_1[i];
          j++;
        }
  }


State* VF2MonoState::Clone()
  { return new VF2MonoState(*this);
  }


void VF2MonoState::BackTrack()
  { assert(core_len - orig_core_len <= 1);
    assert(added_node1 != NULL_NODE);
  
    if (orig_core_len < core_len)
      { int i, node2;

        if (in_1[added_node1] == core_len)
		  in_1[added_node1] = 0;
	    for(i=0; i<g1->InEdgeCount(added_node1); i++)
		  { int other=g1->GetInEdge(added_node1, i);
		    if (in_1[other]==core_len)
			  in_1[other]=0;
		  }
        
		if (out_1[added_node1] == core_len)
		  out_1[added_node1] = 0;
	    for(i=0; i<g1->OutEdgeCount(added_node1); i++)
		  { int other=g1->GetOutEdge(added_node1, i);
		    if (out_1[other]==core_len)
			  out_1[other]=0;
		  }
	    
		node2 = core_1[added_node1];

        if (in_2[node2] == core_len)
		  in_2[node2] = 0;
	    for(i=0; i<g2->InEdgeCount(node2); i++)
		  { int other=g2->GetInEdge(node2, i);
		    if (in_2[other]==core_len)
			  in_2[other]=0;
		  }
        
		if (out_2[node2] == core_len)
		  out_2[node2] = 0;
	    for(i=0; i<g2->OutEdgeCount(node2); i++)
		  { int other=g2->GetOutEdge(node2, i);
		    if (out_2[other]==core_len)
			  out_2[other]=0;
		  }
	    
	    core_1[added_node1] = NULL_NODE;
		core_2[node2] = NULL_NODE;
	    
	    core_len=orig_core_len;
		added_node1 = NULL_NODE;
	  }

  }
