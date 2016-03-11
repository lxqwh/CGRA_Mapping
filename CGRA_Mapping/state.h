/*------------------------------------------------------------
 * state.h
 * 定义匹配过程中间状态的抽象类
 *-----------------------------------------------------------------*/
#ifndef STATE_H
#define STATE_H

#include "argraph.h"


class State
  { 

    public:
      virtual ~State() {} 
      virtual Graph *GetGraph1()=0;
      virtual Graph *GetGraph2()=0;
      virtual bool NextPair(node_id *pn1, node_id *pn2,
                    node_id prev_n1=NULL_NODE, node_id prev_n2=NULL_NODE)=0;
      virtual bool IsFeasiblePair(node_id n1, node_id n2)=0;
      virtual void AddPair(node_id n1, node_id n2)=0;
      virtual bool IsGoal() =0;
      virtual bool IsDead() =0;
      virtual int CoreLen() =0;
      virtual void GetCoreSet(node_id c1[], node_id c2[]) =0;
      virtual State *Clone() =0;  // 将clone改成Clone，保持一致性
     
      virtual void BackTrack() { };
  };


#endif

