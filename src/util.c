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





PairList makePairList(int size){
	PairList ret;
	ret.xs  = malloc(sizeof(Pair) * size);
	ret.len = 0;
	ret.cap = size;
	return ret;
}

int growPairList(PairList* lst, int size){
	if(lst->cap < size){
		Pair* xs  = lst->xs;
		lst->xs   = malloc(sizeof(Pair) * size);
		for(int i = 0; i < lst->len; i++) lst->xs[i] = xs[i];
		lst->cap  = size;
		free(xs);
	}
	return lst->cap;
}

int appendPairList(PairList* lst, Pair x){
	if(lst->len+5 >= lst->cap) growPairList(lst, lst->cap * 2);
	lst->xs[lst->len] = x;
	lst->len++;
	return lst->len-1;
}


void swapPair(Pair* a, Pair* b){
	Pair t = *a;
	*a     = *b;
	*b     = t;
}

void sortPairSubList(PairList* lst, int lo, int hi){
	if(lo < hi){
		int pivot = lst->xs[hi].a;
		int index = lo-1;
	
		for(int i = lo; i < hi; i++){
			if(lst->xs[i].a <= pivot){
				index++;
				swapPair(&lst->xs[index], &lst->xs[i]);
			}
		}
		swapPair(&lst->xs[index+1], &lst->xs[hi]);
		
		
		int p = index+1;
		
		sortPairSubList(lst, lo, p-1);
		sortPairSubList(lst, p+1, hi);
	}
}

void sortPairList(PairList* lst){
	sortPairSubList(lst, 0, lst->len-1);
}

void printPairList(PairList lst){
	printf("====%i/%i====\n", lst.len, lst.cap);
	for(int i = 0; i < lst.len; i++)
		printf("%i %i\n", lst.xs[i].a, lst.xs[i].b);
	printf("\n");
}






BitList	makeBitList	(int size){
	BitList ret;
	ret.size   =  size;
	ret.length = (size / 64) + ((size % 64) != 0);
	ret.bits   = malloc(sizeof(uint64_t) * ret.length);
	for(int i  = 0; i < ret.length; i++) ret.bits[i] = 0;
	return ret;
}

int	checkBitList(BitList* b, int ix){
	if((ix < 0) || (ix >= b->size)) return 0;
	return (b->bits[ix/64] & (1l << (ix % 64))) != 0;
}

void insertBitList(BitList* b, int ix){
	if((ix < 0) || (ix >= b->size)) return;
	b->bits[ix/64] |=  (1l << (ix % 64));
}

void removeBitList(BitList* b, int ix){
	if((ix < 0) || (ix >= b->size)) return;
	b->bits[ix/64] &= ~(1l << (ix % 64));
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


void printBloom128(Bloom128 b){
	printf("[%3i/128] ", bloom128Fill(b));
	for(int i = 0; i < 64; i++) printf("%c", (b.x & (1l << i))? '#' : '_');
	for(int i = 0; i < 64; i++) printf("%c", (b.y & (1l << i))? '#' : '_');
	printf("\n");
}


int		matchLSH(LSH256 a, LSH256 b){
	int ret = 0;
	for(int i = 0; i < 4; i++)
		ret += __builtin_popcountl(a.bits[i] ^ b.bits[i]);
	return ret;
}


BloomList makeBlmList(int size){
	BloomList ret;
	ret.xs		= malloc(sizeof(int) * size);
	ret.fill	= 0;
	ret.size	= size;
	ret.bloom	= (Bloom128){0, 0};
	return ret;
}


int checkBlmList(BloomList* b, int x){
	if(checkBloom128(b->bloom, x))
		// optimization: assume value was inserted recently
		for(int i = b->fill-1; i >= 0; i--)
			if(b->xs[i] == x) return 1;
	return 0;
}


int appendBlmList(BloomList* b, int x){
	if(b->fill+5 >= b->size){
		int* tmp  = b->xs;
		b->size  *= 2;
		b->xs     = malloc(sizeof(int) * b->size);
		for(int i = 0; i < b->fill; i++) b->xs[i] = tmp[i];
		free(tmp);
	}

	if(checkBlmList(b, x)) return 0;
	insertBloom128(&b->bloom, x);
	b->xs[b->fill] = x;
	b->fill++;
	return 1;
}


void printBlmList(BloomList* b){
	printBloom128(b->bloom);
	printf("[%3i/%3i] := \n", b->fill, b->size);
	for(int i = 0; i < b->fill; i++){
		printf("%6i ", b->xs[i]);
		if((i % 16) == 15) printf("\n");
	}
	printf("\n");
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



