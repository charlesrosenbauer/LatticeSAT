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
	printf("FRAME %04i [%08i] <%3i/128>\n", n, f.guess, bloom128Fill(f.bm));
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
	for(int i = 0; i < d->vct; i++) ret.frames[i] = (Frame){(Bloom128){0,0}, makeStack(32), -1};
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
	DecorInstance* inst = psol->inst;
	IntStack        stk = makeStack(32);
	pushStack(&stk, f->guess);	
	
	while(stk.fill){
		int v  = popStack(&stk);
		int vi = v<0? -v : v;
		
		if(v != f->guess){
			pushStack(&f->prop, v);
			psol->infers[vi] = psol->ffill-1;
		}
		
		printf("V=%i\n", vi);
		if((psol->infers[vi] < 0) || (v == f->guess)){
			printf("  ; %i\n", inst->vcsct[vi]);
			for(int  i = 0; i < inst->vcsct[vi]; i++){
				int ci = inst->varcs[vi][i];
				Clause cs = inst->cs[ci];
				// is clause satisfied?
				if((psol->csat[ci/64] & (1l << (ci%64))) || (cs.a == v) || (cs.b == v) || (cs.c == v)){
					psol->csat[ci/64] |= 1l << (ci%64);
					printf("  CI=%i\n", ci);
				}else{
					// clause is not satisfied, decide on potential propagation
					int ax = cs.a<0? -cs.a : cs.a;
					int bx = cs.b<0? -cs.b : cs.b;
					int cx = cs.c<0? -cs.c : cs.c;
					
					int ct = 0;
					ct += (psol->infers[ax] < 0);
					ct += (psol->infers[bx] < 0);
					ct += (psol->infers[cx] < 0);
					if(ct == 1){
						// this needs to handle more complex cases
						if(psol->infers[ax] < 0){
							pushStack(&stk, cs.a);
							printf("push %i\n", cs.a);
						}
						if(psol->infers[bx] < 0){
							pushStack(&stk, cs.b);
							printf("push %i\n", cs.b);
						}
						if(psol->infers[cx] < 0){
							pushStack(&stk, cs.c);
							printf("push %i\n", cs.c);
						}
					}
				}
			}
		}else{
			// contradiction!
			f->prop.fill = 0;
			free(stk.stk);
			return 0;
		}
	}
	
	return f->prop.fill;
}

int pathSolve(PathSolver* psol){
	DecorInstance* inst = psol->inst;
	for(int i = 1; i < inst->vct; i++){
		if(psol->infers[i] < 0){
		
			int n = 0;
			printf("SET   : ");
			for(int j = 1; j < inst->vct; j++){
				if(psol->infers[j] >= 0){
					printf("%i ", (psol->bits[j/64] & (1l << (j%64)))? j : -j);
					n++;
				}
			}
			printf("    [[%i]]\nUNSAT : ", n);
			n = 0;
			for(int j = 0; j < inst->cct; j++){
				if(!(psol->csat[j/64] & (1l << (j%64)))){
					printf("[%i %i %i] ", inst->cs[j].a, inst->cs[j].b, inst->cs[j].c);
					n++;
				}
			}
			printf("    [[%i]]\n", n);
		
		
			psol->bits[i/64] |= 1l << (i%64);
			psol->frames[psol->ffill] = (Frame){
				.prop.fill = 0,
				.guess     = i
			};
			psol->infers[i] = inst->cct+1;
			psol->ffill++;
			if(!unitProp(psol, &psol->frames[psol->ffill-1])){
				printf("BACKTRACK!\n");
			}
		}
	}
	
	int sat = 0;
	for(int i = 0; i < (inst->cct/64)+1; i++)
		sat += __builtin_popcountl(psol->csat[i]);
	return sat;
}

int countSolve(){
	return 0;
}



