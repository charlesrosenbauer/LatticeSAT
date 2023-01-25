#ifndef __UTIL_HEADER__
#define __UTIL_HEADER__


#include "stdint.h"


typedef struct{
	int*		stk;
	int			fill, size;
}IntStack;

IntStack	makeStack		(int);
int			pushStack		(IntStack*, int);
int			popStack		(IntStack*);


typedef struct{
	int*	xs;
	int		len, cap;
}IntList;

IntList makeIntList  (int);
int     growIntList  (IntList*, int);
int     appendIntList(IntList*, int);


typedef struct{
	uint64_t	x, y;
}Bloom128;

uint64_t	hashU64			(uint64_t);
int			subsetBloom128	(Bloom128 , Bloom128);
int			checkBloom128	(Bloom128 , uint64_t);
void		insertBloom128	(Bloom128*, uint64_t);
int			bloom128Fill	(Bloom128);
void		printBloom128	(Bloom128);


typedef struct{
	Bloom128	bloom;
	int*		vals;
	int			fill, size;
}BloomList;

BloomList	makeBloomList	(int);
int			insertBloomList (BloomList*, int);
int			lookupBloomList (BloomList*, int);
int			searchBloomList	(BloomList*, int);
int			indexBloomList	(BloomList*, int);

uint64_t	rng				();
uint64_t	rngseed			(uint64_t, uint64_t);

int			u64Len			(int);

#endif
