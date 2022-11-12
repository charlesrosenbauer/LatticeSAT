#ifndef __GATES_HEADER__
#define __GATES_HEADER__


#include "stdint.h"


typedef enum{
	GK_EQ		= 0,
	GK_NOT		= 1,
	GK_AND		= 2,
	GK_OR		= 3,
	GK_XOR		= 4,
	GK_NAND		= 5,
	GK_NOR		= 6,
	GK_XNOR		= 7,
	GK_0		= 8,
	GK_1		= 9,
	
	GK_DEAD		= 255	// To be removed on cleanup
}GateKind;

typedef struct{
	uint32_t a, b, q;
	GateKind kind;
}Gate;



typedef struct{
	Gate*	gates;
	int		gatect, gatecap, topvar;
}Circuit;


Circuit makeCircuit    (int);
int		growCircuit    (Circuit*, int);
int		wireCircuit    (Circuit*, int);
void    printCircuit   (Circuit*);
int     simplifyCircuit(Circuit*, uint64_t*, uint64_t*);

void    gateOpGK       (Circuit*, int, int, int, int, GateKind);
void    gateAdd        (Circuit*, int, int, int, int, int, int);
void    gateSub        (Circuit*, int, int, int, int, int, int);
void    wireSet        (Circuit*, int, int, uint64_t*);
void    wirePrint      (uint64_t*, int, int);


#endif
