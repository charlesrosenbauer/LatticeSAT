#include "stdint.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"


int main(){
	Instance sat = randomSAT(17, 16);
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
		
		if(f && back){
			ix--;
			continue;
		}else if(back){
			f ^= m;
			flip[ix/64] ^= m;
			back = 0;
		}else{
			back = 0;
		}
		
		sat.bits[ix/64] &= ~m;
		sat.bits[ix/64] |=  p;
		
		int     bk = 0;
		int   pass = 1;
		printf("GUESS %i : %c\n", ix, p? '1' : '0');
		for(int i = 0; i < sat.cct; i++){
			Clause cs = sat.cs[i];
			int a = cs.a < 0? -cs.a : cs.a;
			int b = cs.b < 0? -cs.b : cs.b;
			int c = cs.c < 0? -cs.c : cs.c;
			if((a <= ix) && (b <= ix) && (c <= ix) && pass){
				pass = checkClause(sat, i);
				if(!pass){
					bk = a;
					if((b != ix) && (b > bk) || (bk == ix)) bk = b;
					if((c != ix) && (c > bk)              ) bk = c;
					printf("  BT %i %i %i [%i]\n", cs.a, cs.b, cs.c, bk);
				}
			}
		}
		
		if(pass){
			printf("PASS\n");
			flip[(ix+1)/64] &= ~(1l << ((ix+1)%64));
			ix++;
		}else if(f){
			printf("BACK\n");
			flip[ ix   /64] &= ~m;
			back = 1;
			ix = bk;
		}else{
			printf("FAIL\n");
			flip[ix/64] ^=  m;
		}
	}
	
	printf("%i\n", checkAssignment(sat));
}
