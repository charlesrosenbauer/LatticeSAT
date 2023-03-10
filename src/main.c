#include "stdint.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"


int main(){
	Instance sat = randomSAT(80, 16);
	printSAT(sat);
	
	int      back = 0;
	int        ix = 1;
	uint64_t flip[4];
	uint64_t pred[4];
	for(int i = 0; i < 4; i++) flip[i] = pred[i] = 0;
	
	while((ix > 0) && (ix <= 16)){
		uint64_t m = 1l << (ix%64);
		uint64_t f =  flip[ix/64] & m;
		uint64_t p = (pred[ix/64] & m) ^ f;
		
		if(p && back){
			ix--;
			continue;
		}else{
			back = 0;
		}
		
		sat.bits[ix/64] &= ~m;
		sat.bits[ix/64] |=  p;
		
		int   pass = 1;
		printf("GUESS %i : %c\n", ix, p? '1' : '0');
		
		for(int i = 0; i < sat.cct; i++){
			Clause c = sat.cs[i];
			if((c.a < ix) && (c.b < ix) && (c.c < ix) && pass){
				pass = checkClause(sat, ix);
			}
		}
		
		if(pass){
			printf("PASS\n");
			ix++;
		}else if(f){
			printf("BACK\n");
			flip[ ix   /64] &= ~m;
			back = 1;
			ix--;
			// TODO: we need to rewind to the last variable which contradicts the failing clause
		}else{
			printf("FAIL\n");
			flip[ix/64] ^=  m;
		}
	}
	
	printf("%i\n", checkAssignment(sat));
}
