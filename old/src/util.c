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
	ret.xs  = malloc(sizeof(int) * size);
	ret.len = 0;
	ret.cap = size;
	return ret;
}

int growIntList(IntList* lst, int size){
	if(lst->cap < size){
		int* xs = lst->xs;
		lst->xs = malloc(sizeof(int) * size);
		for(int i = 0; i < lst->len; i++) lst->xs[i] = xs[i];
		lst->cap  = size;
		free(xs);
	}
	return lst->cap;
}

int appendIntList(IntList* lst, int x){
	if(lst->len+5 >= lst->cap) growIntList(lst, lst->cap * 2);
	lst->xs[lst->len] = x;
	lst->len++;
	return lst->len-1;
}



uint64_t RNGSTATEA = 3189571098710891;
uint64_t RNGSTATEB = 8930819035719833;

uint64_t rng(){
	int   rot  =  RNGSTATEA % 64;
	RNGSTATEA  = (RNGSTATEA * 3719871398751981) - 99389152389011;
	RNGSTATEB  = (RNGSTATEB * 9389123890180999) + 83989378713289;
	
	RNGSTATEB  = (RNGSTATEB << rot) | (RNGSTATEB >> (64-rot));
	RNGSTATEA  = (RNGSTATEA >>  17) ^ (RNGSTATEB >> 19) ^ (RNGSTATEA >> 43) ^ RNGSTATEA;
	
	RNGSTATEA += (RNGSTATEB & 0xF100C0000)? 0 : RNGSTATEB;
	return RNGSTATEA ^ RNGSTATEB;
}

uint64_t rngseed(uint64_t x, uint64_t y){
	rng();
	RNGSTATEA ^= x;
	rng();
	RNGSTATEB ^= y;
	rng();
	return rng();
}


int u64Len(int x){
	return (x / 64) + ((x % 64) != 0);
}


uint64_t hashU64(uint64_t val){
	uint64_t hash = val;
	// FIXME: there's a bias in here somewhere
	hash  = (hash * 5159830179831) - 3511938889975819;
	hash ^= (hash >> 37) ^ (hash << 11);
	return hash;
}


int	subsetBloom128(Bloom128 a, Bloom128 b){
	int x = (a.x & b.x) == b.x;
	int y = (a.y & b.y) == b.y;
	return x & y;
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


void printBloom128(Bloom128 b){
	printf("%016lx %016lx\n", b.x, b.y);
}


int bloom128Fill(Bloom128 b){
	return __builtin_popcountl(b.x) + __builtin_popcount(b.y);
}



BloomList makeBloomList(int size){
	BloomList ret;
	ret.bloom = (Bloom128){.x=0, .y=0};
	ret.size  = size;
	ret.fill  = 0;
	ret.vals  = malloc(sizeof(int) * size);
	return ret;
}


int	lookupBloomList(BloomList* b, int x){
	if(checkBloom128(b->bloom, x)){
		for(int i = 0; i < b->fill; i++)
			if(b->vals[i] == x) return 1;
	}
	return 0;
}


int	searchBloomList(BloomList* b, int x){
	for(int i = 0; i < b->fill; i++)
		if(b->vals[i] == x) return 1;
	return 0;
}


int indexBloomList(BloomList* b, int x){
	for(int i = 0; i < b->fill; i++)
		if(b->vals[i] == x) return i;
	return -1;
}


int	insertBloomList (BloomList* b, int x){
	if(b->fill+5 >= b->size){
		int* tmp  = b->vals;
		b->vals   = malloc(sizeof(int) * b->size * 2);
		for(int i = 0; i < b->fill; i++) b->vals[i] = tmp[i];
		b->size  *= 2;
		free(tmp);
	}
	if(lookupBloomList(b, x)) return 0;
	insertBloom128(&b->bloom, x);
	b->vals[b->fill] = x;
	b->fill++;
	return 1;
}





