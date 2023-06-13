#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "graph.h"
#include "gutil.h"
#include "util.h"
#include "sat.h"



Graph makeSATGraph(Instance inst){
	Graph ret;
	ret.nct = inst.vct;
	
	BloomList* blms = malloc(sizeof(BloomList) * ret.nct);
	for(int i = 0; i < ret.nct; i++) blms[i] = makeBlmList(16);
	
	ret.nodes = malloc(sizeof(int) * ret.nct);
	for(int i = 0; i < ret.nct ; i++) ret.nodes[i] = 0;
	for(int i = 0; i < inst.cct; i++){
		int a = inst.cs[i].a;
		int b = inst.cs[i].b;
		int c = inst.cs[i].c;
		a     = (a < 0)? -a : a;
		b     = (b < 0)? -b : b;
		c     = (c < 0)? -c : c;
		a--;
		b--;
		c--;
		ret.nodes[a]++;
		ret.nodes[b]++;
		ret.nodes[c]++;
		
		appendBlmList(&blms[a], b);
		appendBlmList(&blms[a], c);
		appendBlmList(&blms[b], a);
		appendBlmList(&blms[b], c);
		appendBlmList(&blms[c], a);
		appendBlmList(&blms[c], b);
	}
	
	int ect = 0;
	for(int i = 0; i < ret.nct; i++) ect += blms[i].fill;
	
	ret.edges = malloc(sizeof(int) * ect);
	int fill  = 0;
	for(int i = 0; i < ret.nct; i++){
		ret.nodes[i] = fill;
		for(int j = 0; j < blms[i].fill; j++){
			ret.edges[fill] = blms[i].xs[j];
			fill++;
		}
	}

	for(int i = 0; i < ret.nct; i++) free(blms[i].xs);
	free(blms);
	return ret;	
}



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


void moveGraph(GridTable gtab, Graph g){
	int side  = 1l << gtab.side;
	int size  = side * side;
	
	int* xs   = malloc(sizeof(int) * g.nct);
	int* ys   = malloc(sizeof(int) * g.nct);
	
	for(int i = 0; i < side; i++){
		for(int j = 0; j < side; j++){
			int n = (i * side) + j;
			int k = gtab.table[n];
			if(k >= 0){
				xs[k] = j;
				ys[k] = i;
			}
		}
	}
	
	for(int i = 0; i < side; i++){
		for(int j = 0; j < side; j++){
			int n = (i * side) + j;
			int k = gtab.table[n];
			if(k >= 0){
				int cx    = 0;
				int cy    = 0;
				int head  = g.edges[k];
				int tail  = (k+1 < g.nct)? g.edges[k+1] : g.ect;
				for(int h = head; h < tail; h++){
					cx   += xs[h];
					cy   += ys[h];
				}
				int diff = (tail-head);
				cx   += (diff * xs[k]);
				cy   += (diff * ys[k]);
				diff *=  2;
				cx   /=  diff;
				cy   /=  diff;
				
				cx   += (rng() % 4) - 2;
				cy   += (rng() % 4) - 2;
				cx    = (cx < 0)? 0 : (cx >= side)? side-1 : cx;
				cy    = (cy < 0)? 0 : (cy >= side)? side-1 : cy;
				
				int c = (cy*side)+cx;
				if(gtab.table[c] < 0){
					gtab.table[c] = k;
					gtab.table[n] = -1;
				}else{
					int t = gtab.table[c];
					gtab.table[c] = k;
					gtab.table[n] = t;
				}
			}
		}
	}
	
	free(xs);
	free(ys);
}





