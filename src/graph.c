#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "graph.h"
#include "util.h"



Graph randomGraph(int ect, int nct){
	int* as = malloc(sizeof(int) * ect);
	int* bs = malloc(sizeof(int) * ect);
	
	BloomList* blms = malloc(sizeof(BloomList) * nct);
	for(int i = 0; i < nct; i++) blms[i] = makeBlmList(16);
	
	uint64_t mask = roundExp2(nct) - 1;
	for(int i = 0; i < ect; i++){
		int a = rng() & mask;
		int b = rng() & mask;
		
		if((a < nct) && (b < nct) && (a != b)){
			if(b < a){
				int t = a;
				a = b;
				b = t;
			}
			if(!checkBlmList(&blms[a], b)){
				as[i] = a;
				bs[i] = b;
				appendBlmList(&blms[a], b);
				appendBlmList(&blms[b], a);
			}else{
				i--;	// retry
			}
		}else{
			i--;		// retry
		}
	}
	
	
	Graph ret;
	ret.edges = malloc(sizeof(int) * ect * 2);
	ret.nodes = malloc(sizeof(int) * nct);
	int stack = 0;
	for(int i = 0; i < nct; i++){
		ret.nodes[i] = stack;
		for(int j = 0; j < blms[i].fill; j++){
			ret.edges[stack] = blms[i].xs[j];
			stack++;
		}
	}
	ret.ect = stack;
	ret.nct = nct;
	
	for(int i = 0; i < nct; i++) free(blms[i].xs);
	free(blms);
	free(as);
	free(bs);
	return ret;
}


void printGraph(Graph g){
	printf("====[%5i/%5i]====\n", g.nct, g.ect);
	for(int i = 0; i < g.nct; i++){
		int head  = g.nodes[i];
		int limit = (i+1 < g.nct)? g.nodes[i+1] : g.ect;
		printf("%3i [%3i] | ", i, limit-head);
		for(int j = head; j < limit; j++) printf("%3i ", g.edges[j]);
		printf("\n");
	}
}
