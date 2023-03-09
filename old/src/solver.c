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
	int       asgnsize = ((sat->varct+1) / 64) + (((sat->varct+1) % 64) != 0);
	uint64_t*  setbits = malloc(sizeof(uint64_t) * asgnsize);
	uint64_t* flipbits = malloc(sizeof(uint64_t) * asgnsize);
	for(int i = 0; i < asgnsize; i++) sat->assignment[i] = 0;
	for(int i = 0; i < asgnsize; i++) setbits        [i] = 0;
	for(int i = 0; i < asgnsize; i++) flipbits       [i] = 0;
	
	uint32_t* picks = malloc(sizeof(uint32_t) * (sat->varct+1));
	int       head  = 1;
	
	while((head > 0) && (head <= sat->varct)){
		for(int i = 1; i < head; i++){
			// print stack for debug purposes
		}
		
		// exit for now
		head = 0;
	}
	
	return 0;
}



int checkSolution(Instance* sat){
	for(int i = 0; i < sat->clausect; i++){
		
	}
}
