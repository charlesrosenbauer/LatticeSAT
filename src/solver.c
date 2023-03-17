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

/*
	Path solver might need some other metadata to work well -
	* track satisfied clauses
	* group decisions with their propagations, undo groups at a time
*/
PathSolver initPathSolver(DecorInstance* d){
	PathSolver ret;
	ret.inst	= d;
	ret.pred	= malloc(sizeof(uint64_t) * ((d->vct / 64) + 1));
	ret.path	= malloc(sizeof(int     ) *   d->vct * 2);
	ret.fill	= 0;
	ret.size	= d->vct * 2;
	return ret;
}


int	unitProp(PathSolver* psol, int var){
	
	return 0;
}


int	pathSolve(PathSolver* psol){

	return 0;
}
