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
	int cct		= (d->cct / 64) + 1;
	int vct		= (d->vct / 64) + 1;
	ret.inst	= d;
	ret.csat	= malloc(sizeof(uint64_t) *    cct);
	ret.bits	= malloc(sizeof(uint64_t) *    vct);
	ret.frames	= malloc(sizeof(Frame   ) * d->vct);
	ret.infers	= malloc(sizeof(uint64_t) * d->vct);
	ret.ffill	= 0;
	ret.fpeak	= 0;
	
	for(int i = 0; i <    cct; i++) ret.csat  [i] =  0;
	for(int i = 0; i <    vct; i++) ret.bits  [i] =  0;
	for(int i = 0; i < d->vct; i++) ret.frames[i] = (Frame){(Bloom128){0,0}, makeStack(32), -1, -1};
	for(int i = 0; i < d->vct; i++) ret.infers[i] = -1;
	
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
int unitProp(PathSolver* psol, Frame* f){
	return 0;
}

int pathSolve(PathSolver* psol){
	for(int i = 1; i < psol->inst->vct; i++){
		psol->bits[i/64] |= 1l << (i%64);
		for(int j = 0; j < psol->inst->vcsct[i]; j++){
			int    ci = psol->inst->varcs[i][j];
			Clause cs = psol->inst->cs[ci];
			int    vi = -i;
			if((cs.a == vi) || (cs.b == vi) || (cs.c == vi)){
				psol->csat[ci/64] |= 1l << (ci%64);
			}
		}
	}
	
	int sat = 0;
	for(int i = 0; i < (psol->inst->cct/64)+1; i++)
		sat += __builtin_popcountl(psol->csat[i]);
	return sat;
}

int countSolve(){
	return 0;
}



