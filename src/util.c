#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "util.h"


IntStack makeStack(int size){
	IntStack ret;
	ret.stk  = malloc(sizeof(int) * size);
	ret.size = size;
	ret.fill = 0;
	return ret;
}


int	pushStack(IntStack* stk, int val){
	if(stk->fill+5 >= stk->size){
		int* tmp    = stk->stk;
		stk->size  *= 2;
		stk->stk    = malloc(sizeof(int) * stk->size);
		for(int i   = 0; i < stk->fill; i++) stk->stk[i] = tmp[i];
		free(tmp);
	}
	stk->stk[stk->fill] = val;
	stk->fill++;
	return stk->fill-1;
}


int	popStack(IntStack* stk){
	stk->fill--;
	return stk->stk[stk->fill];
}



IntList makeIntList(int size){
	IntList ret;
	ret.xs   = malloc(sizeof(int) * size);
	ret.fill = 0;
	ret.size = size;
	return ret;
}

int growIntList(IntList* lst, int size){
	if(lst->size < size){
		int* xs   = lst->xs;
		lst->xs   = malloc(sizeof(int) * size);
		for(int i = 0; i < lst->fill; i++) lst->xs[i] = xs[i];
		lst->size = size;
		free(xs);
	}
	return lst->size;
}

int appendIntList(IntList* lst, int x){
	if(lst->fill+5 >= lst->size) growIntList(lst, lst->size * 2);
	lst->xs[lst->fill] = x;
	lst->fill++;
	return lst->fill-1;
}


void printIntList(IntList* lst){
	printf("INT LIST [%i/%i]\n  ", lst->fill, lst->size);
	for(int i = 0; i < lst->fill; i++){
		printf("%i, ", lst->xs[i]);
		if(i % 16 == 15) printf("\n  ");
	}
	printf("\n");
}


uint64_t hashU64(uint64_t val){
	uint64_t hash = val;
	hash  = (hash * 5159830179831) - 3511938889975819;
	hash ^= (hash >> 37) ^ (hash << 11);
	return hash;
}


int checkBloom128(Bloom128 b, uint64_t hash){
	hash = hashU64(hash);
	uint64_t x = 1l << ((hash >>  5) % 64);
	uint64_t y = 1l << ((hash >> 17) % 64);
	return (b.x & x) | (b.y & y);
}


void insertBloom128(Bloom128* b, uint64_t hash){
	hash = hashU64(hash);
	uint64_t x = 1l << ((hash >>  5) % 64);
	uint64_t y = 1l << ((hash >> 17) % 64);
	b->x  |= x;
	b->y  |= y;
}


int bloom128Fill(Bloom128 b){
	return __builtin_popcountl(b.x) + __builtin_popcount(b.y);
}


int		matchLSH(LSH256 a, LSH256 b){
	int ret = 0;
	for(int i = 0; i < 4; i++)
		ret += __builtin_popcountl(a.bits[i] ^ b.bits[i]);
	return ret;
}


uint64_t RNGSTATEA = 839157918718971;
uint64_t RNGSTATEB = 999380912389113;

uint64_t rng(){
	int rot;
	RNGSTATEA = (RNGSTATEA * 735189719777) - 38591739817981;
	RNGSTATEB = (RNGSTATEB * 993851971897) ^ 83878838831753;
	
	rot       =  RNGSTATEA % 64;
	RNGSTATEA = (RNGSTATEA >> rot) | (RNGSTATEA << (64-rot));
	RNGSTATEB = (RNGSTATEB >>  17) ^ (RNGSTATEB <<  41);
	
	RNGSTATEB = (RNGSTATEA & 0x8f91)? RNGSTATEB : RNGSTATEB - RNGSTATEA;
	return RNGSTATEA ^ RNGSTATEB;
}

uint64_t rngseed(uint64_t a, uint64_t b){
	uint64_t x = rng();
	RNGSTATEB ^= b;
	x         += rng();
	RNGSTATEA ^= a;
	return x + rng();
}



int loadFile(char* fname, char** buffer, int64_t* fsize){
	FILE*  pFile = fopen (fname,"r");
	size_t result;
	if (pFile == NULL){ printf("Cannot locate file %s.\n", fname); return 0; }
	fseek (pFile , 0 , SEEK_END);
	*fsize = ftell (pFile);
	rewind (pFile);

	/* allocate memory to contain the whole file: */
	*buffer = malloc((sizeof(char)*(*fsize)) + 4);
	if (buffer == NULL) { printf("Memory error\n"); exit(2); }

	/* copy the file into the buffer: */
	result = fread (*buffer,1,(*fsize),pFile);
	if (result != (*fsize)) { printf("Reading error\n"); exit(3); }
	
	fclose(pFile);
	return 1;
}


int dumpFile(char* fname, char* buffer, int64_t fsize){
	FILE *write_ptr = fopen(fname,"wb");
	fwrite(buffer, fsize, 1, write_ptr);
	return 1;
}




uint64_t hashString(char* text){
	uint8_t* txt = (uint8_t*)text;
	int        i = 0;
	uint64_t ret = 93798170789;
	while(txt[i]){
		int rot = (txt[i] ^ ret) % 64;
		ret     = (ret >> rot) | (ret << (64-rot));
		ret    -= (txt[i] * 356189466161110287) + (517846210013319283 ^ i);
        i++;
	}
	ret = ret? ret : i;
	return ret;
}


uint64_t hashStrSize(char* text, int len){
	uint8_t* txt = (uint8_t*)text;
	uint64_t ret = 93798170789;
	int i;
	for(i = 0; i < len; i++){
		int rot = (txt[i] ^ ret) % 64;
		ret     = (ret >> rot) | (ret << (64-rot));
		ret    -= (txt[i] * 356189466161110287) + (517846210013319283 ^ i);
	}
	ret = ret? ret : len;
	return ret;
}


void leftpad(int pad){
	for(int i = 0; i < pad; i++) printf("  ");
}



uint64_t roundExp2(uint64_t x){
	int n = 63 - __builtin_clzl(x);
	n    += (x & ((1l << n)-1)) != 0;
	return 1l << n;
}




int	decWidth(int n){
	int ret = 0;
	while(n){
		ret++;
		n /= 10;
	}
	return ret;
}


int	decimal(int n, int width, char* val){
	int ret = width;
	for(int i = width-1; i >= 0; i--){
		if(n){
			val[i] = (n % 10) + '0';
			n     /= 10;
			ret    = i;
		}else{
			val[i] = ' ';
		}
	}
	return ret;
}



