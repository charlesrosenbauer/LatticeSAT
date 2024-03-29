#include "assert.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"


/*
	Building neighborhood tables should be mostly randomized, with a focus on
	having a complete coverage of the SAT instance while minimizing total
	entropy.
*/
NeighborhoodTable makeNHTab(int size8, int size10){
	NeighborhoodTable ret;
	ret.b256s  = malloc(sizeof(B256 ) * size8 );
	ret.n8s    = malloc(sizeof(NH8  ) * size8 );
	ret.size8  = size8;
	ret.fill8  = 0;
	
	ret.b1024s = malloc(sizeof(B1024) * size10);
	ret.n10s   = malloc(sizeof(NH10 ) * size10);
	ret.size10 = size10;
	ret.fill10 = 0;

	return ret;
}


int insertNH8(NeighborhoodTable* ntab, NH8 n, B256 b){
	
	return -1;
}


int insertNH10(NeighborhoodTable* ntab, NH10 n, B1024 b){

	return -1;
}





void printHistogram(int* xs, int ct){
	float max = xs[0];
	for(int i = 1; i < ct; i++) max = (max < xs[i])? xs[i] : max;
	
	for(int i = 0; i < ct; i++){
		float n = (((float)xs[i]) / max) * 64.0;
		printf("%04i| ", i);
		for(int j = 0; j < 64; j++)
			if(j < n) printf("#");
		printf("\n");
	}
}


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
	
	return (ax == am) || (bx == bm) || (cx == cm);
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
		
		ct    += (ax == am) || (bx == bm) || (cx == cm);
	}
	return ct;
}


void printSAT(Instance sat){
	printf("================%i/%i================\n", sat.vct, sat.cct);
	for(int i = 0; i < sat.cct; i++)
		printf("  %8i   %8i   %8i\n", sat.cs[i].a, sat.cs[i].b, sat.cs[i].c);
}


void printDecorSAT(DecorInstance sat){
	printf("================%i/%i================\n", sat.vct, sat.cct);
	for(int i = 0; i < sat.cct; i++)
		printf("  %8i   %8i   %8i\n", sat.cs[i].a, sat.cs[i].b, sat.cs[i].c);
	for(int i = 1; i <= sat.vct; i++){
		printf("%8i | [%3i]", i, sat.vcsct[i]);
		for(int j = 0; j < sat.vcsct[i]; j++)
			printf("%6i ", sat.varcs[i][j]);
		printf("\n");
	}
	for(int i = 1; i <= sat.vct; i++){
		printf("%8i | ", i);
		printBlmList(&sat.neighbors[i]);
	}
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


DecorInstance sortInstance(Instance sat){
	DecorInstance ret;
	ret.bits	= sat.bits;
	ret.cs		= sat.cs;
	ret.cct		= sat.cct;
	ret.vct		= sat.vct;
	
	
	/*
		TODO:
		1. Sort/group clauses based on variables included
		2. Sort variables based on heuristics (VSIDS, etc.)
		3. Create mapping back to original var mapping
	*/
	for(int i  = 0; i < ret.cct; i++){
		int a  = ret.cs[i].a;
		int b  = ret.cs[i].b;
		int c  = ret.cs[i].c;

		int ai = a < 0? -a : a;
		int bi = b < 0? -b : b;
		int ci = c < 0? -c : c;
		
		if(ai > bi){
			int t  = a;
			int ti = ai;
			a      = b;
			ai     = bi;
			b      = t;
			bi     = ti;
		}
		if(bi > ci){
			int t  = b;
			int ti = bi;
			b      = c;
			bi     = ci;
			c      = t;
			ci     = ti;
		}
		if(ai > bi){
			int t  = a;
			int ti = ai;
			a      = b;
			ai     = bi;
			b      = t;
			bi     = ti;
		}
		
		ret.cs[i].a = a;
		ret.cs[i].b = b;
		ret.cs[i].c = c;
	}
	
	
	// Group clauses by variable
	ret.neighbors = malloc(sizeof(BloomList) * (ret.vct + 1));
	for(int i = 1; i < ret.vct+1; i++) ret.neighbors[i] = makeBlmList(64);
	
	ret.varcs = malloc(sizeof(int*) * (ret.vct + 1));
	ret.vcsct = malloc(sizeof(int ) * (ret.vct + 1));
	for(int i = 0; i < ret.vct+1; i++) ret.vcsct[i] = 0;
	for(int i = 0; i < ret.cct  ; i++){
		Clause cs = ret.cs[i];
		int     a = cs.a < 0? -cs.a : cs.a;
		int     b = cs.b < 0? -cs.b : cs.b;
		int     c = cs.c < 0? -cs.c : cs.c;
		ret.vcsct[a]++;
		ret.vcsct[b] +=  (a != b)             ? 1 : 0;
		ret.vcsct[c] += ((a != c) && (b != c))? 1 : 0;
	}
	for(int i = 1; i < ret.vct+1; i++) ret.varcs[i] = malloc(sizeof(int) * ret.vcsct[i]);
	for(int i = 1; i < ret.vct+1; i++) ret.vcsct[i] = 0;
	for(int i = 0; i < ret.cct  ; i++){
		Clause cs = ret.cs[i];
		int     a = cs.a < 0? -cs.a : cs.a;
		int     b = cs.b < 0? -cs.b : cs.b;
		int     c = cs.c < 0? -cs.c : cs.c;
		
		ret.varcs[a][ret.vcsct[a]] = i;
		ret.vcsct[a]++;
		
		if(a != b){
			ret.varcs[b][ret.vcsct[b]] = i;
			ret.vcsct[b]++;
		}
		
		if((a != c) && (b != c)){
			ret.varcs[c][ret.vcsct[c]] = i;
			ret.vcsct[c]++;
		}
	}
	for(int i = 0; i < ret.cct; i++){
		Clause cs = ret.cs[i];
		int     a = cs.a < 0? -cs.a : cs.a;
		int     b = cs.b < 0? -cs.b : cs.b;
		int     c = cs.c < 0? -cs.c : cs.c;
		
		appendBlmList(&ret.neighbors[a], b);
		appendBlmList(&ret.neighbors[a], c);
		
		appendBlmList(&ret.neighbors[b], a);
		appendBlmList(&ret.neighbors[b], c);
		
		appendBlmList(&ret.neighbors[c], a);
		appendBlmList(&ret.neighbors[c], b);
	}
	
	return ret;
}





int dfsSolver(Instance sat, int* cts){
	if(cts != NULL)
		for(int i = 0; i <= sat.vct; i++) cts[i] = 0;

	int size = (sat.vct / 64) + 1;

	int*     table = alloca(sizeof(uint64_t) * size);
	for(int i = 0; i < size; i++) table[i] = 0;
	
	int       back = 0;
	int         ix = 1;
	uint64_t* flip = alloca(sizeof(uint64_t) * size);
	uint64_t* pred = alloca(sizeof(uint64_t) * size);
	for(int i = 0; i < size; i++) flip[i] = pred[i] = 0;
	
	while((ix > 0) && (ix <= sat.vct)){
		uint64_t m = 1l << (ix%64);
		uint64_t f =  flip[ix/64] & m;
		uint64_t p = (pred[ix/64] & m) ^ f;
		
		if(f && back){
			ix--;
			continue;
		}else if(back){
			f ^= m;
			p ^= m;
			flip[ix/64] ^= m;
			back = 0;
		}else{
			back = 0;
		}
		
		if(cts != NULL) cts[ix]++;
		
		sat.bits[ix/64] &= ~m;
		sat.bits[ix/64] |=  p;
		
		int     bk = 0;
		int   pass = 1;
		//printf("GUESS %i : %c\n", ix, p? '1' : '0');
		for(int i = 0; i < sat.cct; i++){
			Clause cs = sat.cs[i];
			int a = cs.a < 0? -cs.a : cs.a;
			int b = cs.b < 0? -cs.b : cs.b;
			int c = cs.c < 0? -cs.c : cs.c;
			if((a <= ix) && (b <= ix) && (c <= ix) && pass){
				pass = checkClause(sat, i);
				if(!pass){
					bk = a;
					if((b != ix) && (b > bk) || (bk == ix)) bk = b;
					if((c != ix) && (c > bk)              ) bk = c;
					//printf("  BT %i %i %i [%i]\n", cs.a, cs.b, cs.c, bk);
				}
			}
		}
		
		if(pass){
			//printf("PASS\n");
			flip[(ix+1)/64] &= ~(1l << ((ix+1)%64));
			ix++;
		}else if(f){
			//printf("BACK\n");
			flip[ ix   /64] &= ~m;
			back = 1;
			ix = bk;
		}else{
			//printf("FAIL\n");
			flip[ix/64] ^=  m;
		}
	}
	
	return ix > 0;
}



int solver(DecorInstance sat, int* cts){
	/*
		TODO:
		* DFS solver with order invariance
		* add unit propagation
		* eventually support neighborhoods and bitSAT
		* eventually support neighborhood learning
	*/
	int* path = malloc(sizeof(int) * (sat.vct+1));
	for(int i = 0; i < (sat.vct+1); i++) path[i] = 0;
	
	int ix = 1;
	while((ix > 0) && (ix <= sat.vct)){
		
		ix--;
	}
	
	return 0;
}



void solverBenchmark(int head, int tail, int skip, int size){
	int val = 0;
	int max = 0;
	for(int i = head; i < tail; i += skip){
		Instance sat = randomSAT(i, size);
		//printSAT(sat);
	
		int* table = alloca(sizeof(int) * (size + 4));
		dfsSolver(sat, table);
	
		printf("%i\n", checkAssignment(sat));
		printHistogram(table, sat.vct+1);
		
		int t = 0;
		for(int j = 1; j <= sat.vct; j++) t += table[j];
		
		if(val < t){
			val = t;
			max = i;
		}
		printf(">>>> %i %i\n", i, t);
		
		free(sat.cs  );
		free(sat.bits);
	}
	printf("MAX = %i : %i\n", val, max);
}

