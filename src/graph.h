#ifndef __GRAPH_HEADER__
#define __GRAPH_HEADER__


#include "stdint.h"

#include "gutil.h"
#include "util.h"




typedef struct{
	int*	edges;
	int*	nodes;
	
	int		nct, ect;
}Graph;

typedef struct{
	uint32_t*	colors;
	int			size;
}ColorTable;

typedef struct{
	int*	table;
	int		side;
}GridTable;


Graph		randomGraph		(int, int);
void		printGraph		(Graph);
int			greedyColor		(Graph, int*);
int			greedySortColor	(Graph, int*);

int			cliqueBound		(Graph);

GridTable	makeGTab		(int);
ColorTable	makeCTab		(int, uint32_t);
void		drawGraphGrid	(Img, GridTable, Graph, ColorTable);



#endif
