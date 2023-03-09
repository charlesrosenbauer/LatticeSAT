#ifndef __SAT_HEADER__
#define __SAT_HEADER__

#include "stdint.h"


typedef struct{
	int32_t a, b, c;
}Clause;

typedef struct{
	uint64_t*	bits;

	Clause*		cs;
	int			cct, vct;
}Instance;


Instance	randomSAT		(int, int);
int			checkAssignment	(Instance);
void		printSAT		(Instance);











#endif
