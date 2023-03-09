#include "stdint.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"


int main(){
	Instance sat = randomSAT(640, 128);
	printSAT(sat);
	
	
	int temp = 1048576;
	int good = checkAssignment(sat);
	while(temp > 0){
		int accept = ((rng() % 1048576) * 4) < temp;
		
		uint64_t xs[8];
		for(int i = 0; i < 8; i++){
			xs[i] = (rng() % sat.vct) + 1;
			sat.bits[xs[i] / 64] ^= (1l << (xs[i] % 64));
		}
		
		int match = checkAssignment(sat);
		if(accept || (match > good)){
			good = match;
		}else{
			for(int i = 0; i < 8; i++)
				sat.bits[xs[i] / 64] ^= (1l << (xs[i] % 64));
		}
		
		if((temp % 256) == 1) printf("T=%i\n", good);
		if(temp > 262144) temp -= 3;
		temp--;
	}
	
	printf("%i\n", checkAssignment(sat));
}
