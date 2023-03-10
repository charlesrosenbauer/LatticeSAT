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
		
		if((c.a == c.b) || (c.a == c.c) || (c.b == c.c)){
			i--;
			continue;
		}
		
		uint64_t x = rng();
		c.a   = (x & 1)? c.a : -c.a;
		c.b   = (x & 2)? c.b : -c.b;
		c.c   = (x & 4)? c.c : -c.c;
		
		ret.cs[i] = c;
	}

	return ret;
}


int checkClause(Instance sat, int i){
	int ax = sat.cs[i].a;
	int bx = sat.cs[i].b;
	int cx = sat.cs[i].c;
	
	int ai = ax < 0? -ax : ax;
	int bi = bx < 0? -bx : bx;
	int ci = cx < 0? -cx : cx;
		
	ax     = ax > 0?   1 :  0;
	bx     = bx > 0?   1 :  0;
	cx     = cx > 0?   1 :  0;
		
	int am = (sat.bits[ai/64] & (1l << (ai % 64))) != 0;
	int bm = (sat.bits[bi/64] & (1l << (bi % 64))) != 0;
	int cm = (sat.bits[ci/64] & (1l << (ci % 64))) != 0;
	
	return (ax != am) || (bx != bm) || (cx != cm);
}


int	checkAssignment(Instance sat){
	int     ct = 0;
	for(int i  = 0; i < sat.cct; i++){
		int ax = sat.cs[i].a;
		int bx = sat.cs[i].b;
		int cx = sat.cs[i].c;
		
		int ai = ax < 0? -ax : ax;
		int bi = bx < 0? -bx : bx;
		int ci = cx < 0? -cx : cx;
		
		ax     = ax > 0?   1 :  0;
		bx     = bx > 0?   1 :  0;
		cx     = cx > 0?   1 :  0;
		
		int am = (sat.bits[ai/64] & (1l << (ai % 64))) != 0;
		int bm = (sat.bits[bi/64] & (1l << (bi % 64))) != 0;
		int cm = (sat.bits[ci/64] & (1l << (ci % 64))) != 0;
		
		ct    += (ax != am) || (bx != bm) || (cx != cm);
	}
	return ct;
}


void printSAT(Instance sat){
	printf("================%i/%i================\n", sat.vct, sat.cct);
	for(int i = 0; i < sat.cct; i++)
		printf("  %8i   %8i   %8i\n", sat.cs[i].a, sat.cs[i].b, sat.cs[i].c);
}


void annealing(Instance sat){
	/*
		TODO:
		Annealing different parts of the instance in parallel could be a useful
		variation on this, though it would require sorting the graph to maximize
		structure-mapping.
	*/
	int temp = 1048576;
	int good = checkAssignment(sat);
	int init = good;
	uint64_t* bits = malloc(sizeof(uint64_t) * 3);
	for(int i = 0; i < 3; i++) bits[i] = 0;
	
	while(temp > 0){
		int accept = ((rng() % 1048576) * 4) < temp;
		
		for(int i = 0; i < 3; i++) bits[i] = rng() & rng() & rng() & rng();
		for(int i = 0; i < 3; i++) sat.bits[i] ^= bits[i];
		
		int match = checkAssignment(sat);
		if(accept || (match > good)){
			good = match;
		}else{
			for(int i = 0; i < 3; i++) sat.bits[i] ^= bits[i];
		}
		
		//if((temp % 256) == 1) printf("T=%i\n", good);
		if(temp > 262144) temp -= 3;
		temp--;
	}
	
	printf("%i / %i\n", init, checkAssignment(sat));
}

