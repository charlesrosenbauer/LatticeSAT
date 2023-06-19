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
	
	// FIXME : the ret.ect=fill line below here duct-tapes this issue, but there somehow seems to be a bug with this calculation?
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
	ret.ect = fill;

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


void colorCenterGraph(ColorTable ctab, Graph g, int ix){

	for(int i = 0; i < g.nct; i++) ctab.colors[i] = 0;
	ctab.colors[ix] = 1;
	
	int pass  =  0;
	int fill  =  0;
	int last  = -1;
	while((fill  != last) && (fill < g.nct)){
		last = fill;
		for(int i = 0; i < g.nct; i++){
			if(!fill){ i = ix; fill = 1; }
		
			if(ctab.colors[i] >= 0){
				int here  = g.nodes[i];
				int last  = (i+1 < g.nct)? g.nodes[i+1] : g.ect;
				for(int j = here; j < last; j++){
					int n = g.edges[j];
					if(!ctab.colors[n]){
						ctab.colors[n] = ctab.colors[i]+1;
						fill++;
					}
				}
			}
		}
		pass++;
	}
	
	uint32_t max = 1;
	for(int i = 0; i < g.nct; i++){
		max = (ctab.colors[i] > max)? ctab.colors[i] : max;
	}
	uint32_t scale = 0xff00 / max;
	for(int i = 0; i < g.nct; i++){
		ctab.colors[i] = ((max-ctab.colors[i]) * scale) | 0xff;
	}
}



void centerGraph(GridTable gtab, Graph g, int ix){
	ix = (ix < 0)? 0 : (ix >= g.nct)? 0 : ix;
	
	int  fill  = 0;
	int* table = malloc(sizeof(int) * g.nct);
	for(int i  = 0; i < g.nct; i++) table[i] = -1;
	
	int side = 1l << gtab.side;
	int x    = side/2;
	int y    = side/2;
	
	for(int i = 0; i < (side*side); i++) gtab.table[i] = -1;
	
	IntQueue   q = makeQueue(g.nct);
	IntQueue  xs = makeQueue(g.nct);
	IntQueue  ys = makeQueue(g.nct);
	
	while(fill < g.nct){
		pushQueue(&q , ix);
		pushQueue(&xs, x );
		pushQueue(&ys, y );
		
		while(popQueue(&q, &ix) >= 0){
			popQueue(&xs, &x);
			popQueue(&ys, &y);
		
			int tries= 0;
			int cont = 1;
			while(cont){
				int dn = 0;
				int dx, dy;
				if(tries < 5){
					dx = x + (rng() % 5) - 2;
					dy = y + (rng() % 5) - 2;
					dx = (dx < 0)? 0 : (dx >= side)? side-1 : dx;
					dy = (dy < 0)? 0 : (dy >= side)? side-1 : dy;
					dn = (dy * side) + dx;
				}else if(tries < 10){
					dx = x + (rng() % 9) - 4;
					dy = y + (rng() % 9) - 4;
					dx = (dx < 0)? 0 : (dx >= side)? side-1 : dx;
					dy = (dy < 0)? 0 : (dy >= side)? side-1 : dy;
					dn = (dy * side) + dx;
				}else{
					dx = rng() % side;
					dy = rng() % side;
					dn = (dy * side) + dx;
				}
				if( gtab.table[dn] < 0){
					gtab.table[dn] = ix;
					table[ix]      = dn;
					x = dx;
					y = dy;
					
					fill++;
					cont = 0;
				}
				tries++;
			}
		
			int here  = g.nodes[ix];
			int last  = (ix+1 < g.nct)? g.nodes[ix+1] : g.ect;
			for(int i = here; i < last; i++){
				int n = g.edges[i];
				if(table[n] == -1){
					pushQueue(&q , n);
					pushQueue(&xs, x);
					pushQueue(&ys, y);
					table[n] = -2;
				}
			}
		}
		
		if(fill < g.nct){
			for(int i = 0; i < g.nct; i++){
				if(table[i] < 0){
					ix = i;
					i  = g.nct;
					x  = rng() % side;
					y  = rng() % side;
				}
			}
		}
	}
	
	free(q .queue);
	free(xs.queue);
	free(ys.queue);
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





