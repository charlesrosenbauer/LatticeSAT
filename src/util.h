#ifndef __UTIL_HEADER__
#define __UTIL_HEADER__


#include "stdint.h"
#include "stdlib.h"



typedef struct{
	int*		stk;
	int			fill, size;
}IntStack;

IntStack	makeStack		(int);
int			pushStack		(IntStack*, int);
int			popStack		(IntStack*);


typedef struct{
	int*	xs;
	int		fill, size;
}IntList;

IntList makeIntList  (int);
int     growIntList  (IntList*, int);
int     appendIntList(IntList*, int);
void	printIntList (IntList*);


typedef struct{
	uint64_t	x, y;
}Bloom128;

int      checkBloom128 (Bloom128 , uint64_t);
void     insertBloom128(Bloom128*, uint64_t);
int      bloom128Fill  (Bloom128);


typedef struct{
	uint64_t	bits[4];
}LSH256;

int		matchLSH	(LSH256, LSH256);






uint64_t	rng			();
uint64_t	rngseed		(uint64_t, uint64_t);
int			loadFile	(char*, char**, int64_t*);
int			dumpFile	(char*, char*, int64_t);
uint64_t	hashString 	(char*);
uint64_t	hashStrSize	(char*, int);
void    	leftpad		(int);

uint64_t	roundExp2	(uint64_t);
int			decWidth	(int);
int			decimal		(int, int, char*);


#endif
