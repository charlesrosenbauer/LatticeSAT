#include "stdint.h"

#include "util.h"




uint64_t RNGSTATEA = 3189571098710891;
uint64_t RNGSTATEB = 8930819035719833;

uint64_t rng(){
	int   rot  =  RNGSTATEA % 64;
	RNGSTATEA  = (RNGSTATEA * 3719871398751981) - 99389152389011;
	RNGSTATEB  = (RNGSTATEB * 9389123890180999) + 83989378713289;
	
	RNGSTATEB  = (RNGSTATEB << rot) | (RNGSTATEB >> (64-rot));
	RNGSTATEA  = (RNGSTATEA >>  17) ^ (RNGSTATEA >> 43) ^ RNGSTATEA;
	
	RNGSTATEA += (RNGSTATEB & 0xF100C0000)? 0 : RNGSTATEB;
	return RNGSTATEA ^ RNGSTATEB;
}

uint64_t rngseed(uint64_t x, uint64_t y){
	rng();
	RNGSTATEA ^= x;
	rng();
	RNGSTATEB ^= y;
	rng();
	return rng();
}
