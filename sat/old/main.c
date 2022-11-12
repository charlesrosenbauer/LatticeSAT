#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"


#include "sat.h"
#include "gates.h"

#include "SDL/SDL.h"





int main(int argc, char** args){

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Surface* screen = SDL_SetVideoMode(512, 512, 32, 0);
	
	Instance* ins = alloca(sizeof(Instance) * (argc-1));
	
	// Load files
	char** 	 texts = alloca(sizeof(char*  ) * argc);
	int64_t* sizes = alloca(sizeof(int64_t) * argc);
	for(int i = 1; i < argc; i++){
		if(!loadFile(args[i], &texts[i], &sizes[i])){
			printf("Bad file path %s\n", args[i]);
			return -1;
		}
	}
	
	
	// Parse files
	for(int i = 1; i < argc; i++){
		Instance in = parseInstance(texts[i], sizes[i]);
		ins[i-1]    = in;
	}
	
	
	int cont = 1;
	while(cont){
		SDL_Event e;
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_QUIT) cont = 0;
		}
	
	
		SDL_Flip(screen);
		SDL_Delay(16);
	}
	SDL_Quit();
}
