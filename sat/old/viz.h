#ifndef __VIZ_HEADER__
#define __VIZ_HEADER__


#include "stdint.h"


typedef struct{
	int  x,  y;
	int c0, c1;
}Star;


typedef struct{
	Star*	stars;
	int		len;
}Starlist;








#endif
