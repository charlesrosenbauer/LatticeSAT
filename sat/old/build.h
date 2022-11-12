#ifndef __BUILD_HEADER__
#define __BUILD_HEADER__


#include "stdint.h"

#include "sat.h"





typedef enum{
	S_ADD,
	S_SUB,
	
	S_AND,
	S_XOR,
	S_OR,
	S_NOT,
	
	S_ILS,
	S_ILSE,
	S_ULS,
	S_ULSE,
	S_EQ,
	S_NEQ,
	S_NZ,
	
	S_CMOV,
	
	S_EQV
}SMT_Opcode;

typedef enum{
	S_8,
	S_16,
	S_32,
	S_64,
	S_128,
	S_256,
	S_512
}SMT_Size;


#endif
