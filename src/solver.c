#include "assert.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"



/*
	This file is for the path solver, and maybe eventually the lattice solver.
	
	Eventually we might also want a local search system, at least for locating
	high-quality neighborhoods.
	
	A 256-bit local search system might require very different code from an 8,
	12, or 16-bit neighborhood solver. Local search should probably be based
	around a genetic algorithm, with a few thousand generations.
	
	This would be a bit expensive, but we are operating on 256 variables at a
	time and might be able to use neighborhoods as gradients instead of purely
	random mutations.
*/


void printFrame(Frame f, int n){
	printf("FRAME %04i [%08i] <%3i/128>\n", n, f.varAssume, bloom128Fill(f.bm));
	printf("[");
	for(int i = 0; i < f.set.fill; i++) printf("%08i, ", f.set.stk[i]);
	printf("]\n");
}


PathSolver initPathSolver(DecorInstance* d){
	PathSolver ret;
	ret.inst	= d;
	int csize   = ((d->cct / 64) + 1);
	int vsize   = ((d->vct / 64) + 1);
	ret.csat	= malloc(sizeof(uint64_t) * csize);
	for(int i = 0; i <= (d->cct / 64); i++) ret.csat[i] = 0;
	ret.pred	= malloc(sizeof(uint64_t) * vsize);
	ret.shut	= malloc(sizeof(uint64_t) * vsize);
	ret.flip	= malloc(sizeof(uint64_t) * vsize);
	for(int i = 0; i < vsize; i++) ret.pred[i] = 0;
	for(int i = 0; i < vsize; i++) ret.shut[i] = 0;
	for(int i = 0; i < vsize; i++) ret.flip[i] = 0;
	
	ret.frames	= malloc(sizeof(Frame   ) *   d->vct * 2);
	ret.ffill	= 0;
	ret.fpeak	= 0;	// this stores the highest ffill reached, for memory management reasons (freeing stacks)
	return ret;
}


int assume(PathSolver* psol, int var){
	// guess predicted value (possibly flipped), set flip, set shut
	// if flip is flipped back to zero, return 0 (fail)
	// not sure this logic is 100% correct
	int      ix = var / 64;
	uint64_t mx = 1l << (var % 64);
	
	psol->shut[ix] |= mx;
	psol->flip[ix] ^= mx;
	psol->pred[ix] ^= (psol->flip[ix] & mx);
	
	if(psol->flip[ix] & mx) return 1;
	return 0;
}


int checkClauseVars(Clause c, int x){
	int ai = c.a < 0? -c.a : c.a;
	int bi = c.b < 0? -c.b : c.b;
	int ci = c.c < 0? -c.c : c.c;
	return (ai == x) || (bi == x) || (ci == x);
}

int checkClauseVals(Clause c, int x){
	return (c.a == x) || (c.b == x) || (c.c == x);
}


int unitProp(){
	return 0;
}

int pathSolve(){
	return 0;
}

int countSolve(){
	return 0;
}



