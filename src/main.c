#include "stdint.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"


int main(){
	Instance sat = randomSAT(640, 128);
	printSAT(sat);
}
