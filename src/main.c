#include "stdint.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"
#include "gutil.h"

#include "SDL/SDL.h"


/*
	TODO:
	
	Visualization:
	* diagnostics
		* bits/clauses/neighborhoods touched
		* bits/clauses/neighborhood  heatmap
		* bits/clauses/neighborhoods propagated
		* heuristic accuracy
		* hard cores
	* interactivity
	* bit sorting
	
	Solver:
	* lattice solver
	* WFC
*/
int main(){
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Surface* screen = SDL_SetVideoMode(1024, 512, 32, 0);

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
	
	BoxList bl = initBoxList(16);
	appendBox(&bl, (Box){8, 16, 1000,  4, 0x7f7f7f, 1});
	appendBox(&bl, (Box){8, 16, 1000, 16, 0x7f7f7f, 2});
	
	
	int mx   = 0;
	int my   = 0;
	int click= 0;
	int cont = 1;
	while(cont){
		click = 0;
		SDL_Event e;
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_QUIT) cont = 0;
			if(e.type == SDL_MOUSEMOTION){
				mx = e.motion.x;
				my = e.motion.y;
			}
			if(e.type == SDL_MOUSEBUTTONDOWN){
				click = 1;
			}
		}
		
		uint32_t* pix = screen->pixels;
		int ix = 0;
		for(int i = 0; i < 512; i++){
			for(int j = 0; j < 1024; j++){
				if(j < 512){
					int k = ((i/scale) * n) + (j/scale);
					pix[ix] = (k < sat.vct)? ((sat.bits[k/64] & (1l << (k%64)))? 0xff0000 : 0xff) : 0;
				}else{
					pix[ix] = 0;
				}
				int dx = j-mx;
				int dy = i-my;
				if((dx*dx)+(dy*dy) < 53){
					pix[ix] = 0x3f7f9f;
				}
				
				ix++;
			}
		}
		
		for(int i = 0; i < bl.fill; i++) drawBox(pix, 512, 1024, bl.bs[i]);
		if(click){
			int f = checkBox(bl, mx, my);
			if (f) printf("%i\n", f);
		}
		
		printf("SAT=%i/%i\n", pathSolve(&solv, 100), dsat.cct);
	
		SDL_Flip(screen);
		SDL_Delay(30);
	}
	SDL_Quit();
}
