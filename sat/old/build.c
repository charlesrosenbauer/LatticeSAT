#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "sat.h"
#include "build.h"



/*
	AND
	-1 -2 +3
	+1 -3
	+2 -3
	
	OR
	+1 +2 -3
	-1 +3
	-2 +3
	
	NOT
	+1 +2
	-1 -2
	
	EQV
	+1 +2
	-1 -2
	
	XOR
	-1 -2 -3
	-1 +2 +3
	+1 -2 +3
	+1 +2 -3
	
	HALF ADDER
	-1 -2 +3
	+1 -3
	+2 -3
	-1 -2 -4
	-1 +2 +4
	+1 -2 +4
	+1 +2 -4
*/



Instance blankInstance(int lits, int cls){
	Instance ret;
	ret.literals  = malloc(sizeof(int   ) * lits);
	ret.clauses   = malloc(sizeof(Clause) * cls );
	ret.clausect  = 0;
	ret.literalct = 0;
	ret.litsize   = 0;
	ret.litcap    = lits;
	ret.clausecap = cls;
	return ret;
}


int growInstance(Instance* in, int ls, int cs){
	if(ls){
		int* lits    = malloc(sizeof(int   ) * (in->litcap    + ls));
		for(int i    = 0; i < in->litcap;    i++) lits[i] = in->literals[i];
		free(in->literals);
		in->literals = lits;
	}
	if(cs){
		Clause* cls  = malloc(sizeof(Clause) * (in->clausecap + cs));
		for(int i    = 0; i < in->clausecap; i++) cls [i] = in->clauses [i];
		free(in->clauses);
		in->clauses  = cls;
	}
	return ls + cs;
}


void allocInstance(Instance* in, int ls, int cs, int* li, int* ci){
	*ci = in->clausect;
	*li = in->literalct;
	if(((in->literalct + ls) >= in->litcap)
	|| ((in->clausect  + cs) >= in->clausecap))
		growInstance(in, in->litcap, in->clausecap);
	in->literalct += ls;
	in->clausect  += cs;
}


int addAND(Instance* in, int a, int b, int q){
	int li, ci;
	allocInstance(in, 7, 3, &li, &ci);
	in->clauses  [ci  ] = (Clause){li  , 3};
	in->literals [li  ] = -a;
	in->literals [li+1] = -b;
	in->literals [li+2] =  q;
	
	in->clauses  [ci+1] = (Clause){li+3, 2};
	in->literals [li+3] =  a;
	in->literals [li+4] = -q;
	
	in->clauses  [ci+2] = (Clause){li+5, 2};
	in->literals [li+5] =  b;
	in->literals [li+6] = -q;
	return 3;
}

int addOR(Instance* in, int a, int b, int q){
	int li, ci;
	allocInstance(in, 7, 3, &li, &ci);
	in->clauses  [ci  ] = (Clause){li  , 3};
	in->literals [li  ] =  a;
	in->literals [li+1] =  b;
	in->literals [li+2] = -q;
	
	in->clauses  [ci+1] = (Clause){li+3, 2};
	in->literals [li+3] = -a;
	in->literals [li+4] =  q;
	
	in->clauses  [ci+2] = (Clause){li+5, 2};
	in->literals [li+5] = -b;
	in->literals [li+6] =  q;
	return 3;
}

int addXOR(Instance* in, int a, int b, int q){
	int li, ci;
	allocInstance(in, 12, 4, &li, &ci);
	in->clauses  [ci   ] = (Clause){li  , 3};
	in->literals [li   ] = -a;
	in->literals [li+ 1] = -b;
	in->literals [li+ 2] = -q;
	
	in->clauses  [ci+ 1] = (Clause){li  , 3};
	in->literals [li+ 3] =  a;
	in->literals [li+ 4] =  b;
	in->literals [li+ 5] = -q;
	
	in->clauses  [ci+ 2] = (Clause){li  , 3};
	in->literals [li+ 6] =  a;
	in->literals [li+ 7] = -b;
	in->literals [li+ 8] =  q;
	
	in->clauses  [ci+ 3] = (Clause){li  , 3};
	in->literals [li+ 9] = -a;
	in->literals [li+10] =  b;
	in->literals [li+11] =  q;
	return 4;
}

int addNOT(Instance* in, int a, int q){
	int li, ci;
	allocInstance(in, 12, 4, &li, &ci);
	in->clauses  [ci  ] = (Clause){li  , 3};
	in->literals [li  ] = -a;
	in->literals [li+1] = -q;
	
	in->clauses  [ci+1] = (Clause){li  , 3};
	in->literals [li+2] =  a;
	in->literals [li+3] =  q;
	return 2;
}

int addADD(Instance* in, int as, int bs, int c, int qs, int rc, int len){
	int xs    = 0;	// allocate len
	int ys    = 0;	// allocate len
	int zs    = 0;	// allocate len
	int cs    = 0;	// allocate len-2
	int carry = c;
	for(int i = 0; i < len; i++){
		// xs[i] = as[i] & bs[i]
		// ys[i] = as[i] ^ bs[i]
		// qs[i] = ys[i] ^ carry
		// zs[i] = ys[i] & carry
		// carry = cs[i+1]
		// carry = xs[i] | zs[i]
	}
	return 0;
}







