
#ifndef MATCH_H
#define MATCH_H

#include "argraph.h"
#include "state.h"


typedef bool (*match_visitor)(int n, node_id c1[], node_id c2[], 
                              void *usr_data);

bool match(State *s0, int *pn, node_id c1[], node_id c2[]);

int match(State *s0, match_visitor vis, void *usr_data=NULL);

#endif
