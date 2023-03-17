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


PathSolver initPathSolver(DecorInstance* d){
	PathSolver ret;
	ret.inst	= d;
	ret.csat	= malloc(sizeof(uint64_t) * ((d->cct / 64) + 1));
	for(int i = 0; i <= (d->cct / 64); i++) ret.csat[i] = 0;
	ret.pred	= malloc(sizeof(uint64_t) * ((d->vct / 64) + 1));
	ret.path	= malloc(sizeof(int     ) *   d->vct * 2);
	ret.frames	= malloc(sizeof(int     ) *   d->vct * 2);
	ret.pfill	= 0;
	ret.ffill	= 0;
	return ret;
}


int	unitProp(PathSolver* psol, int var){
	DecorInstance* inst = psol->inst;
	for(int i = 0; i < inst->vcsct[var]; i++){
		int  cid = inst->varcs[var][i];
		Clause c = inst->cs[cid];
		if(!(psol->csat[cid/64] & (1l << (cid%64)))){
			// check if exactly one free variable exists in the clause
			int ct = 1;
			if(ct == 1){
				// propagate unit, recurse
			}else if(ct == 0){
				// backtrack!
				// reset all vars in frame
				// return failing clause cid
				return cid;
			}
		}
	}
	
	return 0;
}


int	pathSolve(PathSolver* psol){
	DecorInstance* inst = psol->inst;
	int ct = 1;
	while((ct > 0) && (ct <= psol->inst->vct)){
		// pick variable
		int v = 0;
		int f = unitProp(psol, v);
		if(!f){
			Clause c = inst->cs[f];
			// rewind to f var
		}
		
		ct--;
	}
	
	return 0;
}
