#ifndef __SAT_HEADER__
#define __SAT_HEADER__


#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "util.h"



typedef struct{
	int32_t a, b, c;
}Clause;

typedef struct{
	uint64_t*	assignment;
	Clause*		clauses;
	int			clausect, varct;
}Instance;


typedef struct{
	uint64_t	bits[16];
}Bit1024;


typedef struct{
	Bit1024		bits;
	uint32_t	vars[10];
	
	uint64_t	bloom;
	
	uint32_t*	clauses;
	int			clausect;
	uint16_t	entropy;
	uint8_t		varct;
}NH;

typedef struct{
	Instance	inst;

	// Neighborhoods
	NH*			hoods;
	int			hoodct, hoodcap;
	
	// Clauses per variable
	Bloom128*	blooms;
	uint32_t**	clausemap;
	int*		sizes;
	
	// Neighborhoods per variable
	int**		varnhs;
	int*		nhcts;
}SATTable;



typedef struct{
	uint64_t	old, neu, idx;
}Remap;

typedef struct{
	Remap*		rmap;
	int			fill, size;
	int			bptr;
	
	int64_t		var;
}Stack;

typedef struct{
	SATTable		stab;

	Bit1024*		bittab;
	uint16_t*		enttab;
	int				bfill, bsize;
	
	// We may eventually support additional neighborhoods being added during solving
	uint64_t*		bitmap;
	int				mfill, msize;
	
	Stack*			stack;
	int				sfill, ssize;
}SolverState;



Instance		newInstance		(int, int);
Instance		randomInstance	(int, int);
int				checkInstance	(Instance);
void			printInstance	(Instance);
void			sortInstance	(Instance*);

uint16_t		heuristicLocal	(Bit1024, int*, int*);

uint32_t		varBloom		(int);

SATTable		makeSATTab		(Instance);
void			printSATTab		(SATTable);
int				precheckSAT		(SATTable);

SolverState		makeSolverState	(SATTable);
int				solverStep		(SolverState*);


#endif
