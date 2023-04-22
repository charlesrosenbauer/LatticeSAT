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
	int csize   = ((d->cct / 64) + 1);
	int vsize   = ((d->vct / 64) + 1);
	ret.csat	= malloc(sizeof(uint64_t) * csize);
	for(int i = 0; i <= (d->cct / 64); i++) ret.csat[i] = 0;
	ret.pred	= malloc(sizeof(uint64_t) * vsize);
	ret.shut	= malloc(sizeof(uint64_t) * vsize);
	ret.flip	= malloc(sizeof(uint64_t) * vsize);
	for(int i = 0; i < vsize; i++) ret.shut[i] = 0;
	for(int i = 0; i < vsize; i++) ret.flip[i] = 0;
	
	ret.path	= malloc(sizeof(int     ) *   d->vct * 2);
	ret.frames	= malloc(sizeof(Frame   ) *   d->vct * 2);
	ret.pfill	= 0;
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


int	unitProp(PathSolver* psol, int var){
	DecorInstance* inst = psol->inst;
	IntStack stk = makeStack(64);
	pushStack(&stk, var);
	int ret = 0;
	if(0){
		end:
		free(stk.stk);
		return ret;
	}
	
	while(stk.fill){
		int v = popStack(&stk);
		for(int i = 0; i < inst->vcsct[v]; i++){
			int  cid = inst->varcs[v][i];
			Clause c = inst->cs[cid];
			if(!(psol->csat[cid/64] & (1l << (cid%64)))){	// is this clause open?
				// check if exactly one free variable exists in the clause
				int      ai = c.a < 0? -c.a : c.a;
				int      bi = c.b < 0? -c.b : c.b;
				int      ci = c.c < 0? -c.c : c.c;
				uint64_t av = 1l << (ai % 64);
				uint64_t bv = 1l << (bi % 64);
				uint64_t cv = 1l << (ci % 64);
				uint64_t am = c.a < 0?    0 : av;
				uint64_t bm = c.b < 0?    0 : bv;
				uint64_t cm = c.c < 0?    0 : cv;
			
				int  vx = 0;
				int  ct = 0;
				int sat = 0;
				if(!sat){
					if(ai == v){
						if(!((psol->pred[ai/64] & am) ^ am)){
							sat = 1;
							psol->csat[cid/64] |= (1l << (cid%64));
						}
					}else if(!((psol->pred[ai/64] & am) ^ am)){
						ct++;
						vx = ai;
					}
				}
				if(!sat){
					if(bi == v){
						if(!((psol->pred[bi/64] & bm) ^ bm)){
							sat = 1;
							psol->csat[cid/64] |= (1l << (cid%64));
						}
					}else if(!((psol->pred[bi/64] & bm) ^ bm)){
						ct++;
						vx = bi;
					}
				}
				if(!sat){
					if(ci == v){
						if(!((psol->pred[ci/64] & cm) ^ cm)){
							sat = 1;
							psol->csat[cid/64] |= (1l << (cid%64));
						}
					}else if(!((psol->pred[ci/64] & cm) ^ cm)){
						ct++;
						vx = ci;
					}
				}
				if(!sat && (ct == 1)){
					// propagate unit, recurse
					
					pushStack(&stk, vx);
					pushStack(&psol->frames[psol->ffill].set, vx);
				}else if(ct == 0){
					// backtrack!
					// reset all vars in frame
					// return failing clause cid
					ret = cid;
					goto end;
				}
			}
		}
	}
	
	ret = 0;
	goto end;
}


/*
	* pick a new variable
	* guess its value
	* unit propagate
		* contradiction: rewind
		* okay: continue
		
	solve :
		pick var : x
		assume x = 0
		unit prop
		if complete return SAT
		if contradict
			locate which prop pass set cont var
			backtrack to prop pass
			if prop pass = 0
				assume pass = 1 ...
			else
				backtrack further
		else
			rescurse solve
		
*/
int	pathSolve(PathSolver* psol){
	DecorInstance* inst = psol->inst;
	int ct        = 1;
	psol->path[1] = 1;
	
	while((ct > 0) && (ct <= psol->inst->vct)){
		// pick variable
		psol->path[ct] = 1;	// FIXME later
		
		int f = unitProp(psol, psol->path[ct]);
		if(!f){
			Clause c = inst->cs[f];
			// rewind to f var
		}
		
		ct--;
	}
	
	return 0;
}



/*
	Trying to count the total number of solutions.
	
	Of course, for large SAT problems this is hard.
	
	Rather, it probably makes sense to measure entropy of the solution space - 
	the number of bits required to encode the number of solutions - and likely
	just try to place an upper bound on the solution space.
	
	One simple bound would be to find a minimum collection of neighborhoods
	that fully covers the variable space, generate bit tables for each one, and
	sum the entropy.
	
	Of course, this is not very effective at figuring out how these different
	neighborhoods interact, which is also a very important factor.
	
	Some variant on a DPLL algorithm that continues searching even after
	finding a solution should work. One recursive framing could be:
	
	count :
		ct := 0
		pick var : x
		assume x = 0
		unit prop
		if complete
			ct++
		else if !contradiction
			ct += count()
		rewind
		assume x = 1
		unit prop
		if complete
			ct++
		else if !contradiction
			ct += count()
		return ct
	
	This algorithm would suffice in theory, though in practice any problem with
	a large number of solutions could easily be infeasible to compute. A few
	options on resolving this:
	
	* Leverage BitSAT stuff to count more than 1 at a time. This probably will
		only help so much.
	
	* Have a recursive depth limit and estimate/bound everything below.
	
	* Attempt to solve several parts of a problem independently and find some
		way to combine them.
*/
int countSolve(PathSolver* psol){
	DecorInstance* inst = psol->inst;
	
	
	return 0;
}


