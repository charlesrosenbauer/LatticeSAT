#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "graph.h"
#include "util.h"


int greedyColor(Graph g, int* colors){
	for(int i = 0; i < g.nct; i++) colors[i] = -1;

	int maxcolor = 1;
	for(int i = 0; i < g.nct; i++){
		int here = g.nodes[i];
		int next = (i+1 < g.nct)? g.nodes[i+1] : g.ect;
		Bloom128 b = (Bloom128){0, 0};
		for(int  j = here; j < next; j++){
			if(g.edges[j] < i)
				insertBloom128(&b, colors[g.edges[j]]);
		}
		for(int j = 0; j < maxcolor; j++){
			if(!checkBloom128(b, j)){
				colors[i] = j;
				j = maxcolor;
			}
		}
		if(colors[i] < 0){
			colors[i] = maxcolor;
			maxcolor++;
		}
	}
	
	return maxcolor;
}


int greedySortColor(Graph g, int* colors){
	for(int i = 0; i < g.nct; i++) colors[i] = -1;
	
	PairList pl = makePairList(g.nct+10);
	for(int i = 0; i < g.nct; i++){
		int here = g.nodes[i];
		int next = (i+1 < g.nct)? g.nodes[i+1] : g.ect;
		appendPairList(&pl, (Pair){here-next, i});
	}
	sortPairList(&pl);

	int maxcolor = 1;
	for(int i = 0; i < g.nct; i++){
		int n = pl.xs[i].b;
	
		int here = g.nodes[n];
		int next = (n+1 < g.nct)? g.nodes[n+1] : g.ect;
		Bloom128 b = (Bloom128){0, 0};
		for(int  j = here; j < next; j++){
			if(colors[g.edges[j]] >= 0)
				insertBloom128(&b, colors[g.edges[j]]);
		}
		for(int j = 0; j < maxcolor; j++){
			if(!checkBloom128(b, j)){
				colors[n] = j;
				j = maxcolor;
			}
		}
		if(colors[n] < 0){
			colors[n] = maxcolor;
			maxcolor++;
		}
	}
	
	return maxcolor;
}
