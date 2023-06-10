#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "graph.h"
#include "util.h"



int cliqueBound(Graph g){
	int maxfork = 0;
	for(int i = 0; i < g.nct; i++){
		int here = g.nodes[i];
		int next = (i+1 < g.nct)? g.nodes[i+1] : g.ect;
		int fork = next - here;
		maxfork  = (maxfork < fork)? fork : maxfork;
	}
	int* fill = alloca(sizeof(int) * maxfork);
	for(int i = 0; i < g.nct; i++){
		int here = g.nodes[i];
		int next = (i+1 < g.nct)? g.nodes[i+1] : g.ect;
		int fork = next - here;
		fill[fork]++;
	}
	int max = 0;
	for(int i = 0; i < maxfork; i++){
		if(fill[i] >= i){
			max = i;
		}
	}
	return max;
}
