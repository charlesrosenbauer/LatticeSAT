#ifndef __IMPLICATION_HEADER__
#define __IMPLICATION_HEADER__


#include "stdint.h"


typedef enum{
	TMK_AND		= 1,
	TMK_OR		= 2,
	TMK_NOT		= 3,
	TMK_XOR		= 4,
	TMK_PAR		= 5,
	TMK_IMP		= 6
}TermKind;

typedef struct{
	uint32_t	a, b;
	TermKind	kind;
}Term;

typedef struct{
	Term*		ins,  exs,  proof;
	int			inct, exct, proofsize;
	uint64_t	bloom;
}Implication;


typedef struct{
	Term*	terms;
	int		termct, termcap;
}ImplicationGraph;

ImplicationGraph	makeImplicationGraph(int);
int					insertGraphTerm		(ImplicationGraph*, Term);


/*
	PLAN:
	* build implication graph data structures
	* build checking code
	* add some basic axioms
	* DFS search
	* Stochastic search
	* ML-optimized searches
*/






#endif
