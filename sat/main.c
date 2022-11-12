#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"
#include "viz.h"

#include "SDL/SDL.h"


int main(){

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Surface* screen = SDL_SetVideoMode(1024, 1024, 32, 0);

	int pct = 32;
	Instance inst = randomInstance(pct, pct*4);
	sortInstance(&inst);
	
	SATTable stab = makeSATTab(inst);
	printSATTab(stab);
	if(!precheckSAT(stab)){
		printf("UNSAT\n");
		return 0;
	}
	
	
	SolverState solver = makeSolverState(stab);
	int cont = 1;
	while(cont){
		cont = solverStep(&solver);
		cont = 0;
	}
	
	
	Point* ps = alloca(sizeof(Point) * pct);
	scatterPoints(&inst, ps, pct);
	
	cont = 1;
	while(cont){
		SDL_Event e;
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_QUIT) cont = 0;
		}
		
		drawPoints(screen, ps, pct);
		
		SDL_Flip(screen);
		SDL_Delay(30);
	}
	SDL_Quit();
	
}
