#ifndef __PLOT_HEADER__
#define __PLOT_HEADER__


#include "stdint.h"

#include "sat.h"




typedef struct{
	int32_t* 	vars;
	uint32_t*	colors;
	int			h, w;
}GridPlot;


GridPlot	makeGridPlot	(DecorInstance, int);
void		drawGridPlot	(uint32_t*, int, int, GridPlot);



#endif
