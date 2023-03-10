#include "stdint.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"



int main(){
	Instance sat = randomSAT(64, 16);
	sortInstance(sat);
	printSAT(sat);
}
