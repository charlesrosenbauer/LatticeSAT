#ifndef __SAT_HEADER__
#define __SAT_HEADER__


#include "stdint.h"



typedef struct{
	int	literalStart;
	int	litct;
}Clause;

typedef struct{
	/*
		clausect	:	number of clauses
		literalct	:	number of literals
		litsize		:	size of clause literal array
		litcap		:	allocation cap for literal array
		clausecap	:	allocation cap for clause array
	*/
	int*	literals;
	Clause*	clauses;
	int	clausect, literalct, litsize, litcap, clausecap;
}Instance;



typedef struct{
	uint64_t bits[4];
	uint32_t vars[8];
}Sat8;

typedef struct{
	Sat8*	sat8s;
	uint64_t filt[ 4];
	uint32_t vars[16];
}Sat16;



int      loadFile     (char*, char**, int64_t*);
Instance parseInstance(char*, int);


Instance blankInstance (int, int);
Instance randomInstance(int, int);
void     printInstance (Instance);
int      checkSolution (Instance, uint64_t*);
void     printBits     (uint64_t*, char*, int);
int      growInstance  (Instance*, int, int);
int      solve         (Instance, uint64_t*);

#endif
