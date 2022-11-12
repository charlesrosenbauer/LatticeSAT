#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "sat.h"
#include "util.h"






void solveSAT8(Sat8* s, int* literals, Clause* cls, int clct){
	
	for(int i = 0; i < 8; i++) s->vars[i] = 0;
	for(int i = 0; i < 4; i++) s->bits[i] = 0xffffffffffffffff;
	
	uint8_t* pss = alloca(sizeof(uint8_t) * clct);
	uint8_t* nss = alloca(sizeof(uint8_t) * clct);
	for(int i = 0; i < clct; i++){
		uint8_t ps = 0;
		uint8_t ns = 0;
		for(int j = 0; j < cls->litct; j++){
			int l = literals[cls->literalStart+j];
			if(l){
				uint8_t* xs =  (l < 0)? &ns : &ps;
				l           =  (l < 0)?  -l :   l;
				int      ix = ((l >> 3) ^ (l - 5)) % 8;
				for(int   k = 0; k < 8; k++){
					int   n = (k+ix)%8;
					if(!s->vars[n] || (s->vars[n] == l)){
						s->vars[n] = l;
						ix         = n;
						k          = 8;
					}
				}
				*xs |= 1 << ix;
			}
		}
		pss[i] = ps;
		nss[i] = ns;
	}
	
	
	uint64_t ks[6] = {
		0x5555555555555555,
		0x3333333333333333,
		0x0f0f0f0f0f0f0f0f,
		0x00ff00ff00ff00ff,
		0x0000ffff0000ffff,
		0x00000000ffffffff
	};
	
	for(int i = 0; i < clct; i++){
	
		// TODO: fix this logic
		uint64_t b64 = 0;
		b64 |= (pss[i] &  1)?  ks[0] : 0;
		b64 |= (pss[i] &  2)?  ks[1] : 0;
		b64 |= (pss[i] &  4)?  ks[2] : 0;
		b64 |= (pss[i] &  8)?  ks[3] : 0;
		b64 |= (pss[i] & 16)?  ks[4] : 0;
		b64 |= (pss[i] & 32)?  ks[5] : 0;
		
		b64 |= (nss[i] &  1)? ~ks[0] : 0;
		b64 |= (nss[i] &  2)? ~ks[1] : 0;
		b64 |= (nss[i] &  4)? ~ks[2] : 0;
		b64 |= (nss[i] &  8)? ~ks[3] : 0;
		b64 |= (nss[i] & 16)? ~ks[4] : 0;
		b64 |= (nss[i] & 32)? ~ks[5] : 0;
		
		s->bits[0] &= ((pss[i] &  64) || (nss[i] &  64))? b64 : 0;
		s->bits[1] &= ((pss[i] & 128) || (nss[i] &  64))? b64 : 0;
		s->bits[2] &= ((pss[i] &  64) || (nss[i] & 128))? b64 : 0;
		s->bits[3] &= ((pss[i] & 128) || (nss[i] & 128))? b64 : 0;
	}
}



Instance randomInstance(int vars, int clauses){
	Instance ret;
	ret.literals = malloc(sizeof(int)    * (clauses * 3) + 16);
	ret.clauses  = malloc(sizeof(Clause) *  clauses      + 16);
	for(int i = 0; i < clauses; i++){
		int ix= i * 3;
		int ng= rng();
		ret.literals[ix  ] = ((rng() % vars) + 1) * (ng & 1? 1 : -1);
		ret.literals[ix+1] = ((rng() % vars) + 1) * (ng & 2? 1 : -1);
		ret.literals[ix+2] = ((rng() % vars) + 1) * (ng & 4? 1 : -1);
		ret.clauses [i   ] = (Clause){.literalStart=ix, .litct=3};
	}
	ret.literalct =  vars;
	ret.litsize   =  clauses * 3;
	ret.litcap    = (clauses * 3) + 16;
	ret.clausect  =  clauses;
	ret.clausecap =  clauses      + 16;
	return ret;
}



void printInstance(Instance in){
	for(int i = 0; i < in.clausect; i++){
		Clause c = in.clauses[i];
		printf("#%i : %i %i | ", i, c.litct, c.literalStart);
		for(int j = 0; j < c.litct; j++) printf("%i ", in.literals[c.literalStart+j]);
		printf("\n");
	}
	printf("====%i====%i====\n", in.clausect, in.literalct);
}


int checkSolution(Instance in, uint64_t* bits){
	for(int i = 0; i < in.clausect; i++){
		int pass = 0;
		for(int j = 0; j < in.clauses[i].litct; j++){
			int k = in.literals[in.clauses[i].literalStart+j];
			if(k < 0){
				k    = -k;
				pass = ((1l << (k%64)) & bits[k/64]) == 0;
			}else{
				pass = ((1l << (k%64)) & bits[k/64]) != 0;
			}
			if(pass) break;
		}
		if(!pass) return 0;
	}
	return 1;
}

int measureSolution(Instance in, uint64_t* bits){
	int ret = 0;
	for(int i = 0; i < in.clausect; i++){
		int pass = 0;
		for(int j = 0; j < in.clauses[i].litct; j++){
			int k = in.literals[in.clauses[i].literalStart+j];
			if(k < 0){
				k    = -k;
				pass = ((1l << (k%64)) & bits[k/64]) == 0;
			}else{
				pass = ((1l << (k%64)) & bits[k/64]) != 0;
			}
			if(pass) break;
		}
		ret += pass;
	}
	return ret;
}

void printBits(uint64_t* bs, char* text, int len){
	for(int i = 0; i < len; i++)
		text[i] = ((1l << (i%64)) & bs[i/64])? '#' : '.';
	text[len] = 0;
}


void localPredictor(Instance in, uint64_t* ps){
	int fillsz = (in.literalct / 64) + ((in.literalct % 64) != 0);
	int*   cts = malloc(sizeof(int) * in.literalct);
	for(int i = 0; i < in.literalct; i++) cts[i] = 0;
	
	for(int i = 0; i < in.clausect; i++){
		for(int j = 0; j < in.clauses[i].litct; j++){
			int k = in.literals[in.clauses[i].literalStart+j];
			if(k < 0){
				cts[-k]--;
			}else{
				cts[ k]++;
			}
		}
	}
	
	for(int i = 0; i < in.literalct; i++){
		uint64_t  x = (cts[i] > 0)? 1 : 0;
		ps[i / 64] |= (x << (i % 64));
	}
	free(cts);
}





int solve(Instance in, uint64_t* bs){
	int fillsz = (in.literalct / 64) + ((in.literalct % 64) != 0);
	for(int  i = 0; i < fillsz; i++) bs[i] = 0;
	
	int* clsSat    = malloc(sizeof(int) * in.clausect );
	int* litClsIxs = malloc(sizeof(int) * in.litsize  );
	int* litClsHds = malloc(sizeof(int) * in.literalct);
	int* litClsCts = malloc(sizeof(int) * in.literalct);
	int* litfill   = malloc(sizeof(int) * in.literalct);
	for(int i = 0; i < in.literalct; i++){ litClsCts[i] = 0; litClsHds[i] = 0; litfill[i] = 0;}
	for(int i = 0; i < in.litsize  ; i++)  litClsIxs[i] = 0;
	for(int i = 0; i < in.clausect ; i++)  clsSat   [i] = 0;
	for(int i = 0; i < in.clausect ; i++){
		Clause c = in.clauses[i];
		for(int j = 0; j < c.litct; j++){
			int k = in.literals[c.literalStart+j];
			k     = (k < 0)? -k : k;
			litClsCts[k-1]++;
		}
	}
	int ixhd  = 0;
	for(int i = 0; i < in.literalct; i++){
		litClsHds[i] = ixhd;
		ixhd        += litClsCts[i];
	}
	for(int i = 0; i < in.clausect; i++){
		Clause c = in.clauses[i];
		for(int j = 0; j < c.litct; j++){
			int k = in.literals[c.literalStart+j];
			k     = (k < 0)? -k : k;
			litClsIxs[litClsHds[k-1] + litfill[k-1]] = i;
			litfill[k-1]++;
		}
	}
	
	
	for(int i = 0; i < in.literalct; i++){
		printf("%i: %i (@%i) | ", i+1, litClsCts[i], litClsHds[i]);
		for(int j = 0; j < litClsCts[i]; j++) printf("%i ", litClsIxs[litClsHds[i]+j]);
		printf("\n");
	}
	
	int cont = 0;
	int head = 0;
	while((head >= 0) && (head < in.literalct)){
		int wix = head / 64;
		int bix = head % 64;
		if(0){
			// forward
			head++;
		}else{
			// backtrack
			int set = (bs[wix] & (1l << bix)) != 0;
			if(set){
				set--;
			}else{
				bs[wix] ^= (1l << bix);
			}
		}
		
		
		// walk forward, guessing literals
		// unit propagation
		// maintain table of which literal satisfies each clause
		// * clause is satisfied so long as satisfying literal < head
		// backtrack upon unsatisfiable clause
		// maintain prediction table
		// bitsat eventually
		// commutativity preprocessing eventually
		// vsids eventually
	}
	
	free(clsSat);
	free(litClsIxs);
	free(litClsHds);
	free(litClsCts);
	free(litfill);
	return head < in.literalct;
}
