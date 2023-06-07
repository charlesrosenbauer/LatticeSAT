#ifndef __GUTIL_HEADER__
#define __GUTIL_HEADER__


#include "stdint.h"



typedef struct{
	int h, w, x, y;
	uint32_t color, func;
}Box;

typedef struct{
	Box*	bs;
	int		fill, size;
}BoxList;

typedef struct{
	uint32_t*	pixels;
	int			h, w;
}Img;

Img			makeFont	();



BoxList		initBoxList	(int);
int			insideBox	(Box, int, int);
int			appendBox	(BoxList*, Box);
uint32_t	checkBox	(BoxList , int, int);

void		drawBox		(uint32_t*, int, int, Box);
void		drawText	(Img, Img, char*, int, int, int);



#endif
