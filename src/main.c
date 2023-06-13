#include "string.h"
#include "stdint.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"
#include "gutil.h"
#include "graph.h"

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
	SDL_Surface* screen = SDL_SetVideoMode(1024, 640, 32, 0);
	Img img = (Img){screen->pixels, 640, 1024};
	Img font= makeFont();

	Instance       sat = randomSAT(976, 256);
	DecorInstance dsat = sortInstance(sat);
	printDecorSAT(dsat);
	
	PathSolver solv = initPathSolver(&dsat);
	
	ColorTable ctab = makeCTab(256, 0x0fff0f);
	GridTable  gtab = makeGTab(5);
	{
		int size  = 1 << gtab.side;
		size     *= size;
		size--;
		for(int i = 0; i < 256; i++){
			int n = rng() & size;
			if(gtab.table[n] < 0){
				gtab.table[n] = i;
				int y = (n / 32) * 8;
				ctab.colors[i] = (y * 256) + (255-y);
			}else{
				i--;
			}
		}
	}
	
	
	
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
	
	char msg[256];
	strcpy(msg, "default");
	
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
		
		drawGrid(img, gtab, ctab);
		
		uint32_t* pix = screen->pixels;
		for(int i = 524288; i < 655360; i++) pix[i] = 0x3f3f3f;
		
		for(int i = 0; i < bl.fill; i++) drawBox(pix, 512, 1024, bl.bs[i]);
		if(click){
			int f = checkBox(bl, mx, my);
			if (f) sprintf(msg, "%i\n", f);
		}
		
		//printf("SAT=%i/%i\n", pathSolve(&solv, 100), dsat.cct);
		drawText(img, font, msg, strlen(msg), 520, 16);
	
		SDL_Flip(screen);
		SDL_Delay(30);
	}
	SDL_Quit();
	
	Graph gp = randomGraph(8192, 160);
	printGraph(gp);
	printf("Max clique: %i\n", cliqueBound(gp));
	
	int* colors = alloca(sizeof(int) * gp.nct);
	int colorct = greedyColor(gp, colors);
	printf("====%2i====\n", colorct);
	for(int i   = 0; i < gp.nct; i++)
		printf("%3i | %2i\n", i, colors[i]);
	
	colorct = greedySortColor(gp, colors);
	printf("====%2i====\n", colorct);
	for(int i   = 0; i < gp.nct; i++)
		printf("%3i | %2i\n", i, colors[i]);
}
