#ifndef __TSEYTIN_HEADER__
#define __TSEYTIN_HEADER__


#include "stdint.h"

#include "sat.h"


typedef enum{
	G_EQ	= 0,
	G_NOT	= 1,
	G_AND	= 2,
	G_NAND	= 3,
	G_OR	= 4,
	G_NOR	= 5,
	G_XOR	= 6,
	G_XNOR	= 7
}GateKind;

typedef struct{
	uint32_t a, b, c;
	GateKind k;
}Gate;

typedef struct{
	Gate*	gates;
	int		fill, size, wirect;
}Circuit;



Circuit		allocCircuit(int);
Instance 	circuitToSAT(Circuit);




#endif
