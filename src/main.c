#include "stdint.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"


int main(){
	Instance sat = randomSAT(80, 16);
	printSAT(sat);
	
	printf("%i\n", checkAssignment(sat));
}
