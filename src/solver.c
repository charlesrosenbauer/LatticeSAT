#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "sat.h"


/*
	TODO:
	* start with just a simple, ordinary DPLL solver
	* debug on 8 bit case solved by hand
	* build local neighborhoods to make predictions
	* add predictions, test performance difference and increase difficulty
	* wave function collapse as optimization
	* CDCL-like approach of adding new neighborhoods on big backtracks
*/
int solve(Instance* sat){
	int asgnsize = ((sat->varct+1) / 64) + (((sat->varct+1) % 64) != 0);
	for(int i = 0; i < asgnsize; i++) sat->assignment[i] = 0;
	
	int ix = 1;
	while((ix <= sat->varct) && (ix > 0)){
		ix++;
		
		int  pass = 1;
		// TODO: optimize this later
		for(int i = 0; i < sat->clausect; i++){
			Clause cs = sat->clauses[i];
			int    xa = cs.a < 0? -cs.a : cs.a;
			int    xb = cs.b < 0? -cs.b : cs.b;
			int    xc = cs.c < 0? -cs.c : cs.c;
			
			int   pos = ix+1;
			if((xa >= pos) && (xb >= pos) && (xc >= pos)){
				// check for inconsistency
				xa = (sat->assignment[xa / 64] & (1l << (xa % 64)))? -xa : xa;
				xb = (sat->assignment[xb / 64] & (1l << (xb % 64)))? -xb : xb;
				xc = (sat->assignment[xc / 64] & (1l << (xc % 64)))? -xc : xc;
				if((xa != cs.a) && (xb != cs.b) && (xc != cs.c)){
					pass = 0;
					i    = sat->clausect;
				}
			}
		}
		if(!pass){
			if(sat->assignment[ix / 64] & (1l << (ix % 64))){
				sat->assignment[ix / 64] |= (1l << (ix % 64));
			}else{
				sat->assignment[ix / 64] ^= (1l << (ix % 64));
				ix--;
			}
		}else{
			ix++;
		}
	}
	
	return ix >= sat->varct;
}

