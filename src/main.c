#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"
#include "viz.h"

#include "SDL/SDL.h"


int main(){

	//SDL_Init(SDL_INIT_EVERYTHING);
	//SDL_Surface* screen = SDL_SetVideoMode(1024, 1024, 32, 0);

	int pct = 8;
	Instance inst = randomInstance(pct, pct*4);
	sortInstance(&inst);
	printInstance(inst);
	
	/*
	SATTable stab = makeSATTab(inst);
	printSATTab(stab);
	if(!precheckSAT(stab)){
		printf("UNSAT\n");
		return 0;
	}*/
	
	if(solve(&inst)){
		printf("SAT\n");
		for(int i = 1; i <= inst.varct; i++)
			printf("%c", (inst.assignment[i/64] & (1l << (i % 64)))? '1' : '0');
		printf("\n");
	}else{
		printf("UNSAT\n");
	}
	
}
