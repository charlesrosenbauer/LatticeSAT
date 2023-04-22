#include "stdint.h"
#include "stdio.h"

#include "sat.h"
#include "util.h"



int main(){
	Instance       sat = randomSAT(61, 16);
	DecorInstance dsat = sortInstance(sat);
	printDecorSAT(dsat);
	
	PathSolver solv = initPathSolver(&dsat);
	pathSolve(&solv);
}
