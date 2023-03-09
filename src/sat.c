#include "assert.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"


Instance randomSAT(int cct, int vct){
	assert(vct < cct);
	assert(vct >   1);
	assert(cct >   1);

	Instance ret;
	ret.cct  = cct;
	ret.vct  = vct;
	ret.cs   = malloc(sizeof(Clause)   *   cct);
	ret.bits = malloc(sizeof(uint64_t) * ((vct / 64) + 1));
	
	for(int i = 0; i < cct; i++){
		Clause c;
		c.a   = (rng() % vct) + 1;
		c.b   = (rng() % vct) + 1;
		c.c   = (rng() % vct) + 1;
		
		uint64_t x = rng();
		c.a   = (x & 1)? c.a : -c.a;
		c.b   = (x & 2)? c.b : -c.b;
		c.c   = (x & 4)? c.c : -c.c;
		
		ret.cs[i] = c;
	}

	return ret;
}


int	checkAssignment(Instance sat){

	return 0;
}


void printSAT(Instance sat){


}
