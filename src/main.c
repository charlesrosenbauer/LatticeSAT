#include "stdint.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"


void printHistogram(int* xs, int ct){
	float max = xs[0];
	for(int i = 1; i < ct; i++) max = (max < xs[i])? xs[i] : max;
	
	for(int i = 0; i < ct; i++){
		float n = (((float)xs[i]) / max) * 64.0;
		printf("%04i| ", i);
		for(int j = 0; j < 64; j++)
			if(j < n) printf("#");
		printf("\n");
	}
}


int main(){
	Instance sat = randomSAT(192, 64);
	printSAT(sat);
	
	
	int     table[128];
	for(int i = 0; i < 128; i++) table[i] = 0;
	
	int      back = 0;
	int        ix = 1;
	uint64_t flip[4];
	uint64_t pred[4];
	for(int i = 0; i < 4; i++) flip[i] = pred[i] = 0;
	
	while((ix > 0) && (ix <= sat.vct)){
		uint64_t m = 1l << (ix%64);
		uint64_t f =  flip[ix/64] & m;
		uint64_t p = (pred[ix/64] & m) ^ f;
		
		if(f && back){
			ix--;
			continue;
		}else if(back){
			f ^= m;
			p ^= m;
			flip[ix/64] ^= m;
			back = 0;
		}else{
			back = 0;
		}
		
		table[ix]++;
		
		sat.bits[ix/64] &= ~m;
		sat.bits[ix/64] |=  p;
		
		int     bk = 0;
		int   pass = 1;
		//printf("GUESS %i : %c\n", ix, p? '1' : '0');
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
					//printf("  BT %i %i %i [%i]\n", cs.a, cs.b, cs.c, bk);
				}
			}
		}
		
		if(pass){
			//printf("PASS\n");
			flip[(ix+1)/64] &= ~(1l << ((ix+1)%64));
			ix++;
		}else if(f){
			//printf("BACK\n");
			flip[ ix   /64] &= ~m;
			back = 1;
			ix = bk;
		}else{
			//printf("FAIL\n");
			flip[ix/64] ^=  m;
		}
	}
	
	printf("%i\n", checkAssignment(sat));
	printHistogram(table, sat.vct+1);
}
