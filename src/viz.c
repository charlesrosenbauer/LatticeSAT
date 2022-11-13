#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "SDL/SDL.h"

#include "sat.h"
#include "viz.h"
#include "util.h"




void setColors(Point* pts, int pct, SolverState* ss){
	for(int i = 0; i < pct; i++){
		float s0 = 0;//ss->var0s[i];
		float s1 = 0;//ss->var1s[i];
		if(s0 > s1){
			float tmp = s0;
			s0 = s1;
			s1 = tmp;
		}
		s1 = (s1 < 1)? 1 : s1;
		uint32_t b = (s0 / s1) * 255;
		uint32_t r = 255-b;
		
		pts[i].c = (r << 16) | b;
	}
}


void placePoint(int* ps, int x, int y, Point* pts, int pi){
	for(int i = 0; i < 8; i++){
		int px = (x-4 + rng() % 8) % 512;
		int py = (y-4 + rng() % 8) % 512;
		int ix = (py * 512) + px;
		if(ps[ix] < 0){
			pts[pi] = (Point){px*2, py*2, 0xff};
			ps [ix] = pi;
			return;
		}
	}
	for(int i = 0; i < 65536; i++){
		int px = rng() % 512;
		int py = rng() % 512;
		int ix = (py * 512) + px;
		if(ps[ix] < 0){
			pts[pi] = (Point){px*2, py*2, 0xff};
			ps [ix] = pi;
			return;
		}
	}
}


void scatterPoints(Instance* inst, Point* ps, int pct){
	for(int i = 0; i < pct; i++) ps[i] = (Point){-1, -1, 0xff};
	
	int* pts = malloc(sizeof(int) * 262144);
	for(int i = 0; i < 262144; i++) pts[i] = -1;
	
	for(int i = 0; i < inst->clausect; i++){
		Clause cs = inst->clauses[i];
		int    ai = (cs.a < 0)? -cs.a : cs.a;
		int    bi = (cs.b < 0)? -cs.b : cs.b;
		int    ci = (cs.c < 0)? -cs.c : cs.c;
		Point   p;
		if     (ps[ai].x >= 0)
			p = ps[ai];
		else if(ps[bi].x >= 0)
			p = ps[bi];
		else if(ps[ci].x >= 0)
			p = ps[ci];
		else
			p = (Point){128+(rng()%256), 128+(rng()%256), 0};
		
		if(ps[ai].x < 0)
			placePoint(pts, p.x, p.y, ps, ai);
		if(ps[bi].x < 0)
			placePoint(pts, p.x, p.y, ps, bi);
		if(ps[ci].x < 0)
			placePoint(pts, p.x, p.y, ps, ci);
	}
	for(int i = 0; i < pct; i++)
		if(ps[i].x < 0) placePoint(pts, 0, 0, ps, i);
	
	free(pts);
}


void drawPoints(SDL_Surface* screen, Point* ps, int pct){
	SDL_FillRect(screen, 0, 0);

	uint32_t* pix = screen->pixels;
	for(int i = 0; i < pct; i++){
		for(int j = -1; j < 2; j++){
			for(int k = -1; k < 2; k++){
				int x = k + ps[i].x;
				int y = j + ps[i].y;
				
				if((x < 1024) && (x >= 0)
				&& (y < 1024) && (y >= 0))
					pix[(y * 1024) + x] = ps[i].c;
			}
		}
	}
}
