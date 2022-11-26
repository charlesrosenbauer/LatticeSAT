#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"


int isVarIncluded(NH* n, int v){
	for(int i = 0; i < 10; i++){
		if(n->vars[i] == 0) return 0;
		if(n->vars[i] == v) return 1;
	}
	return 0;
}


Bit1024 and1024(Bit1024 a, Bit1024 b){
	for(int i = 0; i < 16; i++) a.bits[i] &= b.bits[i];
	return a;
}


Bit1024 or1024(Bit1024 a, Bit1024 b){
	for(int i = 0; i < 16; i++) a.bits[i] |= b.bits[i];
	return a;
}


Bit1024 xor1024(Bit1024 a, Bit1024 b){
	for(int i = 0; i < 16; i++) a.bits[i] ^= b.bits[i];
	return a;
}


Bit1024 not1024(Bit1024 a){
	for(int i = 0; i < 16; i++) a.bits[i] = ~a.bits[i];
	return a;
}


int entropy1024(Bit1024 a){
	int   ret = 0;
	for(int i = 0; i < 16; i++) ret += __builtin_popcountl(a.bits[i]);
	return ret;
}


// Assumes normalized to neighborhood
Bit1024 filterByClause(Bit1024 bt, int a, int b, int c, uint8_t flip){
	uint64_t ms[8];
	ms[0] = 0x5555555555555555;
	ms[1] = 0x3333333333333333;
	ms[2] = 0x0f0f0f0f0f0f0f0f;
	ms[3] = 0x00ff00ff00ff00ff;
	ms[4] = 0x0000ffff0000ffff;
	ms[5] = 0x00000000ffffffff;
	ms[6] = 0xffffffffffffffff;
	ms[7] = 0x0000000000000000;
	
	uint16_t ks[4];
	ks[0] = 0x5555;
	ks[1] = 0x3333;
	ks[2] = 0x0f0f;
	ks[3] = 0x00ff;
	
	Bit1024 ax, bx, cx;
	for(int i = 0; i < 16; i++){ ax.bits[i] = 0; bx.bits[i] = 0; cx.bits[i] = 0; }
	
	if(a < 6){
		for(int i = 0; i < 16; i++) ax.bits[i] = ms[a];
	}else{
		for(int i = 0; i < 16; i++) ax.bits[i] = (ms[a-6] & (1 << i))? 0xffffffffffffffff : 0;
	}
	
	if(b < 6){
		for(int i = 0; i < 16; i++) bx.bits[i] = ms[b];
	}else{
		for(int i = 0; i < 16; i++) bx.bits[i] = (ms[b-6] & (1 << i))? 0xffffffffffffffff : 0;
	}
	
	if(c < 6){
		for(int i = 0; i < 16; i++) cx.bits[i] = ms[c];
	}else{
		for(int i = 0; i < 16; i++) cx.bits[i] = (ms[c-6] & (1 << i))? 0xffffffffffffffff : 0;
	}
	
	if(flip & 1) ax = not1024(ax);
	if(flip & 2) bx = not1024(bx);
	if(flip & 4) cx = not1024(cx);
	
	for(int i = 0; i < 16; i++)
		bt.bits[i] = bt.bits[i] & (ax.bits[i] | bx.bits[i] | cx.bits[i]);
	return bt;
}


uint16_t heuristicLocal(Bit1024 bt, int* cts0, int* cts1){
	uint16_t ret = 0;

	uint64_t ms[6];
	ms[0] = 0x5555555555555555;
	ms[1] = 0x3333333333333333;
	ms[2] = 0x0f0f0f0f0f0f0f0f;
	ms[3] = 0x00ff00ff00ff00ff;
	ms[4] = 0x0000ffff0000ffff;
	ms[5] = 0x00000000ffffffff;
	
	for(int i = 0; i < 6; i++){
		int ct0 = 0;
		for(int j = 0; j < 16; j++)
			ct0 += __builtin_popcountl(ms[i] & bt.bits[j]);
		
		int ct1 = 0;
		for(int j = 0; j < 16; j++)
			ct1 += __builtin_popcountl(ms[i] & bt.bits[j]);
			
		cts0[i] += ct0;
		cts1[i] += ct1;
		ret |= (ct0 < ct1) << i;
	}
	
	for(int i = 6; i < 10; i++){
		int ct0 = 0;
		int ct1 = 0;
		
		for(int j = 0; j < 16; j++){
			if(ms[i-6] & (1l << j)){
				ct0 += __builtin_popcountl(bt.bits[j]);
			}else{
				ct1 += __builtin_popcountl(bt.bits[j]);
			}
		}
		
		cts0[i] += ct0;
		cts1[i] += ct1;
		ret |= (ct0 < ct1) < i;
	}
	
	return ret;
}




int insertNHClause(NH* n, uint64_t* vbloom, int* vfill, Clause cs){
	uint64_t bloom   = *vbloom;
	int      fill    = *vfill;
	
	int vs[3];
	vs[0] = (cs.a < 0)? -cs.a : cs.a;
	vs[1] = (cs.b < 0)? -cs.b : cs.b;
	vs[2] = (cs.c < 0)? -cs.c : cs.c;
	
	for(int i = 0; i < 3; i++){
		int vx  = hashU64(vs[i]);
		int v   = vs[i];
		if((bloom & (1l << vx)) && !isVarIncluded(n, v)){
			if(fill >= 8){
				return 0;
			}else{
				bloom |= (1l << vx);
				n->vars[fill] = v;
				fill++;
			}
		}else if(fill >= 8){
			return 0;
		}else{
			bloom |= (1l << vx);
			n->vars[fill] = v;
			fill++;
		}
	}

	*vbloom = bloom;
	*vfill  = fill;
	return 1;
}



void growNeighborTable(SATTable* tab){
	if(tab->hoodct+64 < tab->hoodcap) return;
	NH* tmp       = tab->hoods;
	tab->hoods    = malloc(sizeof(NH) * tab->hoodcap * 2);
	for(int i     = 0; i < tab->hoodct; i++) tab->hoods[i] = tmp[i];
	tab->hoodcap *= 2;
	free(tmp);
}


NH makeNeighborhood(SATTable tab, int seedclause){
	NH ret;
	
	BloomList  vars = makeBloomList(20);
	
	int pivot       = tab.inst.clauses[seedclause].a;
	pivot           = (pivot < 0)? -pivot : pivot; pivot--;
	int cont        = 1;
	int clauselimit = 0;
	int clause      = seedclause;
	while(cont){
		Clause   cs = tab.inst.clauses[clause];
		int       a =  (cs.a < 0)? -cs.a : cs.a; a--;
		int       b =  (cs.b < 0)? -cs.b : cs.b; b--;
		int       c =  (cs.c < 0)? -cs.c : cs.c; c--;
		insertBloomList(&vars, a);
		insertBloomList(&vars, b);
		insertBloomList(&vars, c);
		
		clause = tab.clausemap[a][rng() % tab.sizes[a]];
		
		cont++;
		if(vars.fill > 8){
			cont = 0;
		}else if(cont > 10){
			int p = (pivot != a)? a :
					(pivot != b)? b : c;
			if(p != c) cont = 1;
		}
	}
	if(vars.fill == 9){
		// TODO: eventually we should try to find a tenth variable
	}
	
	for(int i = 0; i < vars.fill; i++) ret.vars[i] = vars.vals[i];
	ret.varct = vars.fill;
	
	int tmp[1024];
	
	ret.clausect = 0;
	for(int i    = 0; i < vars.fill; i++){
		int var  = ret.vars[i];
		for(int j  = 0; j < tab.sizes[var]; j++){
			int ci = tab.clausemap[var][j];
			if(subsetBloom128(tab.blooms[ci], vars.bloom)){
				// Verify that this clause is fully contained
				Clause cs = tab.inst.clauses[ci];
				int     a = (cs.a < 0)? -cs.a : cs.a; a--;
				int     b = (cs.b < 0)? -cs.b : cs.b; b--;
				int     c = (cs.c < 0)? -cs.c : cs.c; c--;
				if(!searchBloomList(&vars, a)) continue;
				if(!searchBloomList(&vars, b)) continue;
				if(!searchBloomList(&vars, c)) continue;
				
				tmp[ret.clausect] = ci;
				ret.clausect++;
			}
		}
	}
	ret.clauses = malloc(sizeof(int) * ret.clausect);
	for(int i = 0; i < ret.clausect; i++) ret.clauses[i] = tmp[i];
	
	/*
		In terms of clause-based constraining, we only need to apply intersections
		when generating neighborhoods the first time. Propagating/constraining
		variables is a different algorithm.
	*/
	int until = 1 << (ret.varct - 6);
	for(int i = 0;     i < until; i++) ret.bits.bits[i] = 0xffffffffffffffff;
	for(int i = until; i < 16   ; i++) ret.bits.bits[i] = 0;
	
	for(int i = 0; i < ret.clausect; i++){
		Clause cs = tab.inst.clauses[ret.clauses[i]];
		int       a =  (cs.a < 0)? -cs.a : cs.a; a--;
		int       b =  (cs.b < 0)? -cs.b : cs.b; b--;
		int       c =  (cs.c < 0)? -cs.c : cs.c; c--;
		
		a           = indexBloomList(&vars, a);
		b           = indexBloomList(&vars, b);
		c           = indexBloomList(&vars, c);
		
		uint8_t flp = ((cs.a < 0)? 1 : 0) |
					  ((cs.b < 0)? 2 : 0) |
					  ((cs.c < 0)? 4 : 0);
		
		ret.bits = filterByClause(ret.bits, a, b, c, flp);
	}
	ret.entropy  = entropy1024(ret.bits);
	
	
	free(vars.vals);
	return ret;
}





SATTable makeSATTab(Instance inst){
	SATTable ret;
	ret.inst	  = inst;
	
	/*
		Build clause map : foreach var, what clauses reference it?
	*/
	ret.blooms    = malloc(sizeof(Bloom128 ) * inst.clausect);
	ret.sizes     = malloc(sizeof(int      ) * inst.varct);
	ret.clausemap = malloc(sizeof(uint32_t*) * inst.varct);
	int* varcts   = malloc(sizeof(int      ) * inst.varct);
	for(int i = 0; i < inst.varct   ; i++) varcts[i] = 0;
	for(int i = 0; i < inst.clausect; i++){
		Clause cs = inst.clauses[i];
		int     a = (cs.a < 0)? -cs.a : cs.a; a--;
		int     b = (cs.b < 0)? -cs.b : cs.b; b--;
		int     c = (cs.c < 0)? -cs.c : cs.c; c--;
		varcts[a]++;
		if (b != a)  varcts[b]++;
		if((c != a)  && (c != b)) varcts[c]++;
	}
	for(int i = 0; i < inst.varct; i++){ ret.clausemap[i] = malloc(sizeof(uint32_t) * varcts[i]); ret.sizes[i] = 0; }
	for(int i = 0; i < inst.clausect; i++){
		Clause cs = inst.clauses[i];
		int     a = (cs.a < 0)? -cs.a : cs.a; a--;
		int     b = (cs.b < 0)? -cs.b : cs.b; b--;
		int     c = (cs.c < 0)? -cs.c : cs.c; c--;
		
		Bloom128 bm;
		insertBloom128(&bm, a);
		insertBloom128(&bm, b);
		insertBloom128(&bm, c);
		ret.blooms[i] = bm;
		
		ret.    clausemap[a][ret.sizes[a]] = i;
		ret.    sizes[a]++;
		if (b != a){
			ret.clausemap[b][ret.sizes[b]] = i;
			ret.sizes[b]++;
		}
		if((c != a)  && (c != b)){
			ret.clausemap[c][ret.sizes[c]] = i;
			ret.sizes[c]++;
		}
	}
	free(varcts);
	
	
	/*
		Build neighborhood map : foreach clause, generate a neighborhood around that clause.
		
		Neighborhoods should have 8-10 variables, preferably 10.
		
	*/
	ret.hoods	  = malloc(sizeof(NH) * inst.clausect);
	ret.hoodct    = 0;
	ret.hoodcap   = inst.clausect;
	
	int   usesize = ((inst.clausect / 64) + 1);
	uint64_t* use = malloc(sizeof(uint64_t) * usesize);
	for(int i = 0; i < usesize      ; i++) use[i] = 0;
	for(int i = 0; i < inst.clausect; i++){
		if(!(use[i/64] & (1l << (i % 64)))){
			NH n = makeNeighborhood(ret, i);
			for(int j = 0; j < n.clausect; j++){
				int k = n.clauses[j];
				use[k/64] |= (1l << (k%64));
			}
			ret.hoods[ret.hoodct] = n;
			ret.hoodct++;
		}
	}
	free(use);
	
	
	/*
		Build mapping from variables to neighborhoods
	*/
	ret.nhcts = malloc(sizeof(int) * inst.varct);
	for(int i = 0; i < inst.varct; i++) ret.nhcts[i] = 0;
	for(int i = 0; i < ret.hoodct; i++){
		for(int j = 0; j < ret.hoods[i].varct; j++)
			ret.nhcts[ret.hoods[i].vars[j]]++;
	}
	ret.varnhs = malloc(sizeof(int*) * inst.varct);
	for(int i = 0; i < inst.varct; i++) ret.varnhs[i] = malloc(sizeof(int) * ret.nhcts[i]);
	for(int i = 0; i < inst.varct; i++) ret.nhcts [i] = 0;
	for(int i = 0; i < ret.hoodct; i++){
		for(int j = 0; j < ret.hoods[i].varct; j++){
			int ix = ret.hoods[i].vars[j];
			ret.varnhs[ix][ret.nhcts[ix]] = i;
			ret.nhcts [ix]++;
		}
	}
	
	return ret;
}


int precheckSAT(SATTable stab){
	for(int i = 0; i < stab.hoodct; i++)
		if(!stab.hoods[i].entropy) return 0;
	return 1;
}


void printSATTab(SATTable tab){
	printf("CLAUSEMAP:\n");
	for(int i = 0; i < tab.inst.varct; i++){
		printf("VAR %04x [%04x] | ", i, tab.sizes[i]);
		for(int j = 0; j < tab.sizes[i]; j++)
			printf("%04x ", tab.clausemap[i][j]);
		printf("\n");
	}
	
	printf("HOODMAP:\n");
	for(int i = 0; i < tab.hoodct; i++){
		printf("HOOD %04x: (%i/%i) :\n", i, tab.hoods[i].entropy, 1 << tab.hoods[i].varct);
		printf("  VARS [%02i] : ", tab.hoods[i].varct);
		for(int j = 0; j < tab.hoods[i].varct; j++) printf("%04x ", tab.hoods[i].vars[j]);
		printf("\n");
		for(int j = 0; j < 16; j++){
			printf("%016lx ", tab.hoods[i].bits.bits[j]);
			if((j % 4) == 3) printf("\n");
		}
		printf("  CLAUSES [%04i] : ", tab.hoods[i].clausect);
		for(int j = 0; j < tab.hoods[i].clausect; j++) printf("%04x ", tab.hoods[i].clauses[j]);
		printf("\n\n");
	}
}



SolverState makeSolverState(SATTable stab){
	SolverState ret;
	ret.stab	= stab;
	
	// Bit table
	ret.bittab	= malloc(sizeof(Bit1024 ) * stab.hoodct * 2);
	ret.enttab	= malloc(sizeof(uint16_t) * stab.hoodct * 2);
	ret.bfill	= stab.hoodct;
	ret.bsize	= stab.hoodct * 2;
	for(int i = 0; i < ret.bfill; i++){
		ret.bittab[i] = stab.hoods[i].bits;
		ret.enttab[i] = stab.hoods[i].entropy;
	}
	
	int solsize		= (stab.inst.varct / 64) + ((stab.inst.varct % 64) != 0);
	ret.solution	= malloc(sizeof(uint64_t) * solsize);
	ret.solset		= malloc(sizeof(uint64_t) * solsize);
	ret.solfill		= 0;;
	return ret;
}



int solverStep(SolverState* solver){
	int minent = 1024;
	int minidx = 0;
	
	/*
		TODO:
		* select neighborhood with lowest entropy
		* find variable with strongest bias, collapse to favored state
		* propagate to all other neighborhoods that reference that variable
		* collapse each neighborhood, find any newly constant variables, propagate those
		* if any neighborhood drops to zero entropy, backtrack
		
		For now:
		* entropy measurement
		* per-variable bias
		* propagation/collapse code
	*/
	
	printf("====STEP: [%i/%i]====\n", solver->solfill, solver->stab.inst.varct);
	
	
	return 1;
}


/*
	Next steps:
	* propagation
	* hypothesis
	* backtracking
	* regression testing
	* visualization
	* gates and tseytin xforms
	* bytecode -> gates -> SAT -> inference
*/

