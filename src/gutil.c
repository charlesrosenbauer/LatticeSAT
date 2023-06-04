#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"

#include "gutil.h"




BoxList initBoxList(int size){
	BoxList ret;
	ret.bs		= malloc(sizeof(Box) * size);
	ret.size	= size;
	ret.fill	= 0;
	return ret;
}


int insideBox(Box b, int x, int y){
	return (x >=  b.x)
	&&     (y >=  b.y)
	&&     (x <= (b.x + b.w))
	&&     (y <= (b.y + b.h));
}


int appendBox(BoxList* bl, Box b){
	if(bl->fill+5 >= bl->size){
		Box* tmp   = bl->bs;
		bl->size  *= 2;
		bl->bs     = malloc(sizeof(Box) * bl->size);
		for(int  i = 0; i < bl->fill; i++) bl->bs[i] = tmp[i];
		free(tmp);
	}
	bl->bs[bl->fill] = b;
	bl->fill++;
	return bl->fill-1;
}


uint32_t checkBox(BoxList bl, int x, int y){
	for(int i = 0; i < bl.fill; i++)
		if(insideBox(bl.bs[i], x, y)) return bl.bs[i].func;
	return 0;
}



void drawBox(uint32_t* px, int h, int w, Box b){
	for(int i = 0; i < b.h; i++){
		int y = i + b.y;
		if((y >= 0) && (y < h)){
			for(int j = 0; j < b.w; j++){
				int x = j + b.x;
				int n = (y * w) + x;
				if((x >= 0) || (x <= w)) px[n] = b.color;
			}
		}
	}
}