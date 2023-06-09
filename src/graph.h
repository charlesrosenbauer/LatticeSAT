#ifndef __GRAPH_HEADER__
#define __GRAPH_HEADER__


#include "stdint.h"

#include "util.h"




typedef struct{
	int*	edges;
	int*	nodes;
	
	int		nct, ect;
}Graph;


Graph	randomGraph	(int, int);
void	printGraph	(Graph);


#endif
