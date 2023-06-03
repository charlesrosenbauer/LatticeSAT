#include "stdint.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"

#include "SDL/SDL.h"



int main(){
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Surface* screen = SDL_SetVideoMode(512, 512, 32, 0);

	Instance       sat = randomSAT(976, 256);
	DecorInstance dsat = sortInstance(sat);
	printDecorSAT(dsat);
	
	PathSolver solv = initPathSolver(&dsat);
	
	
	int n = sat.vct;
	{
		while((n * n) > sat.vct){
			n /= 2;
		}
	}
	printf("RT = %i\n", n);
	int scale = 512 / n;
	
	
	int cont = 1;
	while(cont){
		SDL_Event e;
		while(SDL_PollEvent(&e)) if(e.type == SDL_QUIT) cont = 0;
		
		uint32_t* pix = screen->pixels;
		int ix = 0;
		for(int i = 0; i < 512; i++){
			for(int j = 0; j < 512; j++){
				int k = ((i/scale) * n) + (j/scale);
				pix[ix] = (k < sat.vct)? ((sat.bits[k/64] & (1l << (k%64)))? 0xff0000 : 0xff) : 0;
				ix++;
			}
		}
		
		printf("SAT=%i/%i\n", pathSolve(&solv, 100), dsat.cct);
	
		SDL_Flip(screen);
		SDL_Delay(30);
	}
	SDL_Quit();
}
