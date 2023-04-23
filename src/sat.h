#ifndef __SAT_HEADER__
#define __SAT_HEADER__

#include "stdint.h"

#include "util.h"


typedef struct{
	int32_t a, b, c;
}Clause;

typedef struct{
	uint64_t*	bits;

	Clause*		cs;
	int			cct, vct;
}Instance;

typedef struct{
	uint64_t*	bits;
	Clause*		cs;
	int			cct, vct;
	
	BloomList*	neighbors;
	int*		vmap;
	int**		varcs;
	int*		vcsct;
}DecorInstance;


/*
	Path solver should have some heuristics for picking next variables, as well
	as using unit propagation.
*/
typedef struct{
	Bloom128 bm;
	IntStack set;
	
	int varAssume;
}Frame;

typedef struct{
	DecorInstance*	inst;
	uint64_t*		csat;
	uint64_t*		pred;	// predicted/current value
	uint64_t*		shut;	// currently assumed
	uint64_t*		flip;	// if 1, failing requires a backtrack
	
	int*			path;
	Frame*			frames;
	int				pfill, ffill, fpeak;
}PathSolver;

PathSolver		initPathSolver	(DecorInstance*);
int				unitProp		(PathSolver*, Frame*);
int				pathSolve		(PathSolver*);


Instance		randomSAT		(int, int);
int				checkAssignment	(Instance);
void			printSAT		(Instance);
int				checkClause		(Instance, int);
DecorInstance	sortInstance	(Instance);
int				solver			(DecorInstance, int*);
int				dfsSolver		(Instance, int*);
void			solverBenchmark	(int, int, int, int);
void			printDecorSAT	(DecorInstance);

void			printHistogram	(int*, int);







#endif
