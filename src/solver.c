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
	//for(int i = 0; i < f.set.fill; i++) printf("%08i, ", f.set.stk[i]);
	printf("]\n");
}


PathSolver initPathSolver(DecorInstance* d){
	PathSolver ret;
	ret.inst	= d;
	ret.csat	= malloc(sizeof(uint64_t) * ((d->cct / 64) + 1));
	ret.bits	= malloc(sizeof(uint64_t) * ((d->vct / 64) + 1));
	ret.frames	= malloc(sizeof(Frame   ) *   d->vct);
	ret.ffill	= 0;
	ret.fpeak	= 0;
	
	return ret;
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

/*
	Next steps for sat solver:
	* build a unit propagation solver that is as simple as possible
	* save path solving for lattice solver
	* build primitive semi-recursive count solver
	* build lattice solver, bit manipulation code
	* make all this interactive and visual
*/
int unitProp(){
	return 0;
}

int pathSolve(){
	return 0;
}

int countSolve(){
	return 0;
}



