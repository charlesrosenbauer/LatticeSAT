#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "assert.h"

#include "sat.h"
#include "util.h"





Instance	newInstance(int varct, int clsct){
	Instance ret;
	ret.assignment	= malloc(sizeof(uint64_t) * u64Len(varct));
	ret.clauses		= malloc(sizeof(Clause  ) * clsct);
	ret.clausect	= clsct;
	ret.varct		= varct;
	return ret;
}

Instance	randomInstance(int varct, int clsct){
	Instance ret = newInstance(varct, clsct);
	for(int i = 0; i < clsct; i++){
		uint8_t sign    =  (rng() >> 47);
		int32_t a       = ((rng() >> 19) % varct) + 1;
		int32_t b       = ((rng() >> 17) % varct) + 1;
		int32_t c       = ((rng() >> 23) % varct) + 1;
		while(a == b) a = ((rng() >> 19) % varct) + 1;
		while(a == c) c = ((rng() >> 23) % varct) + 1;
		while(b == c) b = ((rng() >> 17) % varct) + 1;
		a              *=  (sign & 1)?  -1 : 1;
		b              *=  (sign & 2)?  -1 : 1;
		c              *=  (sign & 4)?  -1 : 1;
		ret.clauses[i]  = (Clause){a, b, c};
	}
	
	return ret;
}


void qsortInstance(Instance* inst, int lo, int hi){
	if((lo >= hi) || (lo < 0)) return;
	
	Clause pivot = inst->clauses[hi];
	int ix = lo-1;
	
	for(int i = lo; i < hi-1; i++){
		int xa = inst->clauses[i].a;
		xa     = (xa < 0)? -xa : xa;
		
		int ya = pivot.a;
		ya     = (ya < 0)? -ya : ya;
	
		if(xa < ya){
			ix = ix+1;
			Clause tmp = inst->clauses[ix];
			inst->clauses[ix] = inst->clauses[i];
			inst->clauses[i ] = tmp;
		}
	}
	ix++;
	Clause tmp = inst->clauses[ix];
	inst->clauses[ix] = inst->clauses[hi];
	inst->clauses[hi] = tmp;
	
	qsortInstance(inst, lo, ix  );
	qsortInstance(inst, ix+1, hi);
}


void sortInstance(Instance* inst){
	for(int i = 0; i < inst->clausect; i++){
		Clause cs = inst->clauses[i];
		int     a = (cs.a < 0)? -cs.a : cs.a;
		int     b = (cs.b < 0)? -cs.b : cs.b;
		int     c = (cs.c < 0)? -cs.c : cs.c;
		if((a >= b) && (a >= c)) continue;
		if((a >= b) || (b <  c)){
			// swap A and C
			int tmp = cs.a;
			cs.a    = cs.c;
			cs.c    = tmp;
		}else if(b >= c){
			// swap A and B
			int tmp = cs.a;
			cs.a    = cs.b;
			cs.b    = tmp;
		}
	}
	qsortInstance(inst, 0, inst->clausect-1);
}



int checkInstance(Instance inst){
	for(int i = 0; i < inst.clausect; i++){
		Clause cs = inst.clauses[i];
		int     a = (cs.a < 0)? -cs.a : cs.a;
		int     b = (cs.b < 0)? -cs.b : cs.b;
		int     c = (cs.c < 0)? -cs.c : cs.c;
		
		int    ax = inst.assignment[a/64] & (1l << (a%64));
		if((ax && (cs.a > 0)) || (!ax && (cs.a < 0))) continue;
		
		int    bx = inst.assignment[b/64] & (1l << (b%64));
		if((bx && (cs.b > 0)) || (!bx && (cs.b < 0))) continue;
		
		int    cx = inst.assignment[c/64] & (1l << (c%64));
		if((cx && (cs.c > 0)) || (!cx && (cs.c < 0))) continue;
		
		return 0;
	}
	
	return 1;
}



void printInstance(Instance inst){
	printf("================%i %i================\n", inst.varct, inst.clausect);
	for(int i = 0; i < inst.clausect; i++){
		Clause c = inst.clauses[i];
		printf("%4i : %6i %6i %6i\n", i, c.a, c.b, c.c);
	}
}


uint32_t varBloom(int x){
	uint32_t ret =  0;
	ret         |= 1l << (x % 32);
	x			 = (x >> 5);
	ret			|= 1l << (x % 32);
	return ret;
}
























