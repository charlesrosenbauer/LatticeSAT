#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "graph.h"
#include "gutil.h"



ColorTable makeCTab(int size, uint32_t def){
	ColorTable ret;
	ret.colors = malloc(sizeof(uint32_t) * size);
	ret.size   = size;
	for(int i  = 0; i < size; i++) ret.colors[i] = def;
	return ret;
}


GridTable makeGTab(int side){
	GridTable ret;
	int size  = 1l << side;
	ret.table = malloc(sizeof(int) * size * size);
	ret.side  = side;
	for(int i = 0; i < (size*size); i++) ret.table[i] = -1;
	return ret;
}


void drawGrid(Img img, GridTable gtab, ColorTable ctab){
	int side  = 1l << gtab.side;
	for(int i = 0; i < 512; i++){
		int y = (i * side / 512);
		if( i < img.h ){
			for(int j = 0; j < 512; j++){
				int x = (j * side / 512);
				int n = (y * side ) + x;
				int p = (i * img.w) + j;
				if( j < img.w ){
					int ix = gtab.table[n];
					if((ix >= 0) && (ix < ctab.size)){
						img.pixels[p] = ctab.colors[ix];
					}else{
						img.pixels[p] = 0;
					}
				}
			}
		}
	}
}
