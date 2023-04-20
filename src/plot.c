#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "plot.h"
#include "util.h"
#include "sat.h"





GridPlot makeGridPlot(DecorInstance sat, int iter){
	GridPlot ret;
	ret.h		= sat.vct;
	ret.w		= 1;
	while(ret.h > ret.w){
		ret.h	= (ret.h + 1) / 2;
		ret.w  *= 2;
	}
	
	int* poss	= malloc(sizeof(int) * sat.vct);
	
	int size    = ret.h * ret.w;
	ret.vars	= malloc(sizeof( int32_t) * size);
	ret.colors	= malloc(sizeof(uint32_t) * sat.vct);
	for(int i   = 0; i < size; i++){
		ret.colors[i] = 0;
		if(i+1 <= sat.vct){
			ret.vars[i] = i+1;
			poss    [i] = i;
		}else{
			ret.vars[i] = -1;
		}
	}
	
	/*
	for(int i = 0; i < iter; i++){
		int x = rng() % ret.w;
		int y = rng() % ret.h;
		
		float dx = 0;
		float dy = 0;
		int n = poss[(y * ret.w) + x];
		if( n > 0 ){
			for(int j = 0; j < sat.vcsct[n]; j++){
				Clause cs = sat.varcs[i][j];
				int a = cs.a < 0? -cs.a : cs.a;
				int b = cs.b < 0? -cs.b : cs.b;
				int c = cs.c < 0? -cs.c : cs.c;
				
				if(a){
					int px = poss[a-1] % ret.w;
					int py = poss[a-1] / ret.w;
					
					
				}
			}
		}else{
			i--;
		}
	}*/
	
	free(poss);
	return ret;
}




void drawGridPlot(uint32_t* px, int h, int w, GridPlot p){
	for(int i = 0; i < p.h; i++){
		for(int j = 0; j < p.w; j++){
			
			
		}
	}
}





