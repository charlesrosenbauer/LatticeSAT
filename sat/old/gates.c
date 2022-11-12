#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "gates.h"


Circuit makeCircuit(int gct){
	Circuit ret;
	ret.gates   = malloc(sizeof(Gate) * gct);
	ret.gatect  = 0;
	ret.gatecap = gct;
	ret.topvar  = 0;
	return ret;
}


int growCircuit(Circuit* c, int gs){
	if((c->gatect + gs) >= c->gatecap){
		Gate* gates = malloc(sizeof(Gate) * (c->gatecap + gs) * 2);
		for(int i   = 0; i < c->gatect; i++) gates[i] = c->gates[i];
		free(c->gates);
		c->gates    = gates;
	}
	c->gatect += gs;
	return c->gatect - gs;
}


int wireCircuit(Circuit* c, int ws){
	c->topvar += ws;
	return c->topvar - ws;
}


void gateOpGK(Circuit* c, int ai, int bi, int qi, int len, GateKind gk){
	int gi = growCircuit(c, len);
	for(int i = 0; i < len; i++)
		c->gates[gi+i] = (Gate){ai+i, bi+i, qi+i, gk};
}


void wireSet(Circuit* c, int ix, int len, uint64_t* vals){
	int gi = growCircuit(c, len);
	for(int i = 0; i < len; i++){
		GateKind gk = (vals[i/64] & (1l << (i%64)))? GK_1 : GK_0;
		c->gates[gi+i] = (Gate){0, 0, ix+i, gk};
	}
}


void    wirePrint(uint64_t* vals, int ix, int len){
	char*  cs = alloca(sizeof(char) * (len+1));
	for(int i = 0; i < len; i++)
		cs[len-(i+1)] = (vals[(ix+i)/64] & (1l << ((ix+i)%64)))? '1' : '0';
	cs[len] = 0;
	printf("%s\n", cs);
}


void gateAdd(Circuit* cr, int ai, int bi, int c, int qi, int co, int len){
	int xi  = wireCircuit(cr, len);
	int yi  = wireCircuit(cr, len);
	int zi  = wireCircuit(cr, len);
	int csi = wireCircuit(cr, len-1);
	int gi  = growCircuit(cr, len*5);
	for(int i = 0; i < len; i++){
		int ci = csi+i-1;
		int cni= csi+i;
		if      (i == 0) ci  = c;
		if(i == (len-1)) cni = co;
		
		cr->gates[gi  ] = (Gate){ai+i, bi+i, xi+i, GK_AND};
		cr->gates[gi+1] = (Gate){ai+i, bi+i, yi+i, GK_XOR};
		cr->gates[gi+2] = (Gate){ci  , yi+i, qi+i, GK_XOR};
		cr->gates[gi+3] = (Gate){ci  , yi+i, zi+i, GK_AND};
		cr->gates[gi+4] = (Gate){xi+i, zi+i, cni , GK_OR };
		
		gi += 5;
	}
}

void gateSub(Circuit* cr, int ai, int bi, int c, int qi, int co, int len){
	int bni = wireCircuit(cr, len);
	int cn  = wireCircuit(cr, 1);
	
	int gs  = growCircuit(cr, len);
	for(int i = 0; i < len; i++) cr->gates[gs+i] = (Gate){bi+i, 0, bni+i, GK_NOT};
	int gn  = growCircuit(cr, 1);
	cr->gates[gn] = (Gate){c, 0, cn, GK_NOT};
	
	gateAdd(cr, ai, bni, cn, qi, co, len);
}


int simplifyCircuit(Circuit* c, uint64_t* vals, uint64_t* sets){
	//	Eventually, maybe, add Karnaugh maps, probably with some kind of cache
	//  TODO: fix cases were A = B
	int     passct = 0;
	int      again = 1;
	int   lastgate = c->gatect;
	int      vsize = (c->topvar / 64) + ((c->topvar % 64) != 0);
	for(int i = 0; i < vsize; i++){ sets[i] = 0; vals[i] = 0; }
	while(again){
		//printf("%i %i\n", passct, lastgate);
		again = 0;
		int nextlastgate = 0;
		for(int i = 0; i < lastgate; i++){
			Gate g = c->gates[i];
			//printf("  GATE=%i\n", g.kind);
			uint64_t sa = sets[g.a / 64] & (1l << (g.a % 64));
			uint64_t sb = sets[g.b / 64] & (1l << (g.b % 64));
			uint64_t sq = sets[g.q / 64] & (1l << (g.q % 64));
			
			uint64_t va = vals[g.a / 64] & (1l << (g.a % 64));
			uint64_t vb = vals[g.b / 64] & (1l << (g.b % 64));
			uint64_t vq = vals[g.q / 64] & (1l << (g.q % 64));
			switch(g.kind){
				case GK_EQ	: {
					if      (sa && !sq){
						sets[g.q / 64]         |= (1l << (g.q % 64));
						if(va)  vals[g.q / 64] |= (1l << (g.q % 64));
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if(sq && !sa){
						sets[g.a / 64]         |= (1l << (g.a % 64));
						if(vq)  vals[g.a / 64] |= (1l << (g.a % 64));
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if(sq && sa){
						if(!(va && vq) && (va || vq)) return 0;
					}
				}break;
				
				case GK_NOT : {
					if      (sa && !sq){
						sets[g.q / 64]         |= (1l << (g.q % 64));
						if(!va) vals[g.q / 64] |= (1l << (g.q % 64));
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if(sq && !sa){
						sets[g.a / 64]         |= (1l << (g.a % 64));
						if(!vq) vals[g.a / 64] |= (1l << (g.a % 64));
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if(sq && sa){
						if((va && vq) || !(va || vq)) return 0;
					}
				}break;
				
				case GK_AND : {
					// if the output is on, we can deduce inputs
					if(sq && vq){
						// a = 1; b = 1
						if(sa && !va) return 0;
						if(sb && !vb) return 0;
						if(!(sa && sb)){
							sets[g.a / 64] |= 1l << (g.a % 64);
							vals[g.a / 64] |= 1l << (g.a % 64);
							sets[g.b / 64] |= 1l << (g.b % 64);
							vals[g.b / 64] |= 1l << (g.b % 64);
						}
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if(sa && sb){	// if both inputs are set, we can deduce the output
						// q = a && b
						sets[g.q / 64] |= 1l                   << (g.q % 64);
						vals[g.q / 64] |= ((uint64_t)sa && sb) << (g.q % 64);
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if((sa && !va) || (sb && !vb)){	// if one input is set, we can sometimes deduce the output
						sets[g.q / 64] |= 1l  << (g.q % 64);
						c->gates[i].kind = GK_DEAD; again = 1;	
					}else if(sa && va){		// if we can't deduce the output, we can at least xform the gate
						c->gates[i] = (Gate){g.b, 0, g.q, GK_EQ};
						again = 1;
					}else if(sb && vb){
						c->gates[i] = (Gate){g.a, 0, g.q, GK_EQ};
						again = 1;
					}
				}break;
				
				case GK_OR  : {
					if(sq && !vq){
						// a = 0; b = 0
						if(sa && va) return 0;
						if(sb && vb) return 0;
						if(!(sa && sb)){
							sets[g.a / 64] |= 1l << (g.a % 64);
							sets[g.b / 64] |= 1l << (g.b % 64);
						}
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if(sa && sb){	// if both inputs are set, we can deduce the output
						// q = a || b
						sets[g.q / 64] |= 1l                   << (g.q % 64);
						vals[g.q / 64] |= ((uint64_t)sa || sb) << (g.q % 64);
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if((sa && va) || (sb && vb)){	// if one input is set, we can sometimes deduce the output
						sets[g.q / 64] |= 1l  << (g.q % 64);
						vals[g.q / 64] |= 1l  << (g.q % 64);
						c->gates[i].kind = GK_DEAD; again = 1;	
					}else if(sa && !va){		// if we can't deduce the output, we can at least xform the gate
						c->gates[i] = (Gate){g.b, 0, g.q, GK_EQ};
						again = 1;
					}else if(sb && !vb){
						c->gates[i] = (Gate){g.a, 0, g.q, GK_EQ};
						again = 1;
					}
				}break;
				
				case GK_XOR : {
					// if two inputs are set, we can deduce the third
					int sct = (sa != 0) + (sb != 0) + (sq != 0);
					int vct = (va != 0) + (vb != 0) + (vq != 0);
					if(sct == 2){
						uint64_t val = vct & 1;
						if     (sa && sb){
							sets[g.q / 64] |= 1l << (g.q % 64);
							vals[g.q / 64] |= val<< (g.q % 64);
						}else if(sa && sq){
							sets[g.b / 64] |= 1l << (g.b % 64);
							vals[g.b / 64] |= val<< (g.b % 64);
						}else{
							sets[g.a / 64] |= 1l << (g.a % 64);
							vals[g.a / 64] |= val<< (g.a % 64);
						}
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if(sct == 3){
						if(vct & 1) return 0;	// if all inputs are set, we may detect an UNSAT
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if(sct == 1){		// if we can't deduce the output, we can at least xform the gate
						GateKind gk = vct? GK_NOT : GK_EQ;
						if      (sa){
							c->gates[i] = (Gate){g.b, 0, g.q, gk};
						}else if(sb){
							c->gates[i] = (Gate){g.a, 0, g.q, gk};
						}else{
							c->gates[i] = (Gate){g.a, 0, g.b, gk};
						}
					}
				}break;
				
				case GK_NAND: {
					// if the output is on, we can deduce inputs
					if(sq && !vq){
						// a = 1; b = 1
						if(sa && !va) return 0;
						if(sb && !vb) return 0;
						if(!(sa && sb)){
							sets[g.a / 64] |= 1l << (g.a % 64);
							vals[g.a / 64] |= 1l << (g.a % 64);
							sets[g.b / 64] |= 1l << (g.b % 64);
							vals[g.b / 64] |= 1l << (g.b % 64);
						}
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if(sa && sb){	// if both inputs are set, we can deduce the output
						// q = a && b
						uint64_t val    = (sa && sb) != 0;
						sets[g.q / 64] |= 1l  << (g.q % 64);
						vals[g.q / 64] |= val << (g.q % 64);
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if((sa && !va) || (sb && !vb)){	// if one input is set, we can sometimes deduce the output
						sets[g.q / 64] |= 1l  << (g.q % 64);
						vals[g.q / 64] |= 1l  << (g.q % 64);
						c->gates[i].kind = GK_DEAD; again = 1;	
					}else if(sa && va){		// if we can't deduce the output, we can at least xform the gate
						c->gates[i] = (Gate){g.b, 0, g.q, GK_NOT};
						again = 1;
					}else if(sb && vb){
						c->gates[i] = (Gate){g.a, 0, g.q, GK_NOT};
						again = 1;
					}
				}break;
				
				case GK_NOR : {
					if(sq && vq){
						// a = 0; b = 0
						if(sa && va) return 0;
						if(sb && vb) return 0;
						if(!(sa && sb)){
							sets[g.a / 64] |= 1l << (g.a % 64);
							sets[g.b / 64] |= 1l << (g.b % 64);
						}
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if(sa && sb){	// if both inputs are set, we can deduce the output
						uint64_t val    = (sa || sb) != 0;
						sets[g.q / 64] |= 1l  << (g.q % 64);
						vals[g.q / 64] |= val << (g.q % 64);
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if((sa && va) || (sb && vb)){	// if one input is set, we can sometimes deduce the output
						sets[g.q / 64] |= 1l  << (g.q % 64);
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if(sa && !va){		// if we can't deduce the output, we can at least xform the gate
						c->gates[i] = (Gate){g.b, 0, g.q, GK_NOT};
						again = 1;
					}else if(sb && !vb){
						c->gates[i] = (Gate){g.a, 0, g.q, GK_NOT};
						again = 1;
					}
				}break;
				
				case GK_XNOR: {
					// if two inputs are set, we can deduce the third
					int sct = (sa != 0) + (sb != 0) + (sq != 0);
					int vct = (va != 0) + (vb != 0) + (vq != 0);
					if(sct == 2){
						uint64_t val = (vct & 1) == 0;
						if     (sa && sb){
							sets[g.q / 64] |= 1l << (g.q % 64);
							vals[g.q / 64] |= val<< (g.q % 64);
						}else if(sa && sq){
							sets[g.b / 64] |= 1l << (g.b % 64);
							vals[g.b / 64] |= val<< (g.b % 64);
						}else{
							sets[g.a / 64] |= 1l << (g.a % 64);
							vals[g.a / 64] |= val<< (g.a % 64);
						}
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if(sct == 3){
						if(!(vct & 1)) return 0;	// if all inputs are set, we may detect an UNSAT
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if(sct == 1){		// if we can't deduce the output, we can at least xform the gate
						GateKind gk = vct? GK_EQ : GK_NOT;
						if      (sa){
							c->gates[i] = (Gate){g.b, 0, g.q, gk};
						}else if(sb){
							c->gates[i] = (Gate){g.a, 0, g.q, gk};
						}else{
							c->gates[i] = (Gate){g.a, 0, g.b, gk};
						}
					}
				}break;
				
				case GK_0   : {
					if(!sq){
						sets[g.q / 64] |= (1l << (g.q % 64));
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if(sq && vq) return 0;
				}break;
				
				case GK_1   : {
					if(!sq){
						sets[g.q / 64] |= (1l << (g.q % 64));
						vals[g.q / 64] |= (1l << (g.q % 64));
						c->gates[i].kind = GK_DEAD; again = 1;
					}else if(sq && vq) return 0;
				}break;
				
				case GK_DEAD: break;
			}
			
			if(g.kind != GK_DEAD) nextlastgate = i;
		}
		lastgate = nextlastgate;
		passct  += 1;
	}
	
	int fill = 0;
	for(int i = 0; i < lastgate; i++){
		if(c->gates[i].kind != GK_DEAD){
			c->gates[fill] = c->gates[i];
			fill++;
		}
	}
	c->gatect = fill;
	
	return 1;	// Circuit can be simplified and is valid
}


/*
	Circuit simplification:
	
	q=a&b
		q=1 : a=b=1
		a=0 : q=0
		b=0 : q=0
	q=a|b
		q=0	: a=b=0
		a=1 : q=1
		b=1 : q=1
	q=!a
		q=0 : a=1
		q=1 : a=0
		a=0 : q=1
		a=1 : q=0
	q=!a&b
		q=0 : a=b=1
		a=0 : q=1
		b=0 : q=1
	q=!a|b
		q=1	: a=b=0
		a=1 : q=0
		b=1 : q=0
	q=a
		q=0 : a=0
		q=1 : a=1
		a=0 : q=0
		a=1 : q=1
		
*/
void printCircuit(Circuit* c){
	for(int i = 0; i < c->gatect; i++){
		Gate g = c->gates[i];
		switch(g.kind){
			case GK_EQ	: printf("EQ   : %i %i\n"   , g.a,      g.q); break;
			case GK_NOT : printf("NOT  : %i %i\n"   , g.a,      g.q); break;
			case GK_AND : printf("AND  : %i %i %i\n", g.a, g.b, g.q); break;
			case GK_OR  : printf("OR   : %i %i %i\n", g.a, g.b, g.q); break;
			case GK_XOR : printf("XOR  : %i %i %i\n", g.a, g.b, g.q); break;
			case GK_NAND: printf("NAND : %i %i %i\n", g.a, g.b, g.q); break;
			case GK_NOR : printf("NOR  : %i %i %i\n", g.a, g.b, g.q); break;
			case GK_XNOR: printf("XNOR : %i %i %i\n", g.a, g.b, g.q); break;
			case GK_0   : printf("=0   : %i\n"      ,           g.q); break;
			case GK_1   : printf("=1   : %i\n"      ,           g.q); break;
			case GK_DEAD: printf("XXXX\n"); break;
		}
	}
}





