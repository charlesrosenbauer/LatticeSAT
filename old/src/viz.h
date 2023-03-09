#ifndef __VIZ_HEADER__
#define __VIZ_HEADER__


#include "stdint.h"

#include "SDL/SDL.h"

#include "sat.h"


typedef struct{
	int   x, y;
	uint32_t c;
}Point;




void drawPoints		(SDL_Surface*, Point*, int);
void scatterPoints	(Instance*, Point*, int);
void setColors		(Point*, int, SolverState*);

#endif
