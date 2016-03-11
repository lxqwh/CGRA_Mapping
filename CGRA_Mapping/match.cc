
#include "argraph.h"
#include "match.h"
#include "state.h"
#include "error.h"


static bool match(int *pn, node_id c1[], node_id c2[], State *s);

static bool match(node_id c1[], node_id c2[], match_visitor vis, 
                 void *usr_data, State *s, int *pcount); 



bool match(State *s0, int *pn, node_id c1[], node_id c2[])
  { 
    return match(pn,c1,c2,s0);
  }


int match(State *s0, match_visitor vis, void *usr_data)
  { Graph *g1=s0->GetGraph1();
    Graph *g2=s0->GetGraph2();
	
    int n;
    if (g1->NodeCount()<g2->NodeCount())
      n=g2->NodeCount();
    else
      n=g1->NodeCount();

    node_id *c1=new node_id[n];
    node_id *c2=new node_id[n];

    if (!c1 || !c2)
      error("Out of memory");

    int count=0;
    match(c1, c2, vis, usr_data, s0, &count);

    delete[] c1;
    delete[] c2;
    return count;
  }




static bool match(int *pn, node_id c1[], node_id c2[], State *s)
  { if (s->IsGoal())
      { 
        *pn=s->CoreLen();
        s->GetCoreSet(c1, c2);
        return true;
      }

    if (s->IsDead())
      return false;

    node_id n1=NULL_NODE, n2=NULL_NODE;
    bool found=false;
    while (!found && s->NextPair(&n1, &n2, n1, n2))
      { if (s->IsFeasiblePair(n1, n2))
          { State *s1=s->Clone();
            s1->AddPair(n1, n2);
            found=match(pn, c1, c2, s1);
            s1->BackTrack();
            delete s1;
          }
      }
    return found;
  }


static bool match(node_id c1[], node_id c2[], 
                  match_visitor vis, void *usr_data, State *s, int *pcount)
  { if (s->IsGoal())
      { ++*pcount;
        int n=s->CoreLen();
        s->GetCoreSet(c1, c2);
        return vis(n, c1, c2, usr_data);
      }

    if (s->IsDead())
      return false;

    node_id n1=NULL_NODE, n2=NULL_NODE;
    while (s->NextPair(&n1, &n2, n1, n2))
      { if (s->IsFeasiblePair(n1, n2))
          { State *s1=s->Clone();
            s1->AddPair(n1, n2);
            if (match(c1, c2, vis, usr_data, s1, pcount))
              { s1->BackTrack();
                delete s1;
                return true;
              }
            else
	      { s1->BackTrack();
                delete s1;
	      }
          }
      }
    return false;
  }




