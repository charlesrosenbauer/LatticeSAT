#include "implication.h"

#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"



ImplicationGraph makeImplicationGraph(int sz){
	ImplicationGraph ret;
	ret.terms   = malloc(sizeof(Term) * sz);
	ret.termct  = 0;
	ret.termcap = sz;
	return ret;
}



int	insertGraphTerm	(ImplicationGraph* g, Term t){
	if(g->termcap + 5 >= g->termct){
		// TODO: growImplicationGraph
	}
	g->terms[g->termct] = t;
	g->termct++;
	return g->termct-1;
}
