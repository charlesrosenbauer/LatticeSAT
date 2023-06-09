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
