#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "sat.h"






int loadFile(char* fname, char** buffer, int64_t* fsize){
	FILE*  pFile = fopen (fname,"r");
	size_t result;
	if (pFile == NULL){ printf("Cannot locate file %s.\n", fname); return 0; }
	fseek (pFile , 0 , SEEK_END);
	*fsize = ftell (pFile);
	rewind (pFile);

	// allocate memory to contain the whole file:
	*buffer = malloc((sizeof(char)*(*fsize)) + 4);
	if (buffer == NULL) { printf("Memory error\n",stderr); exit(2); }

	// copy the file into the buffer:
	result = fread (*buffer,1,(*fsize),pFile);
	if (result != (*fsize)) { printf("Reading error\n",stderr); exit(3); }
	
	buffer[0][*fsize] = 0;
	
	fclose(pFile);
	return 1;
}





int parseInt(char* text, int* ix){
	int  ret = 0;
	int sign = 1;
	while(text[*ix] > ' '){
		if(text[*ix] == '-'){
			sign = -1;
		}else{
			ret *= 10;
			ret += text[*ix] - '0';
		}
		(*ix)++;
	}
	//printf("%i\n", ret * sign);
	return ret * sign;
}


int parseLine(char* text, int* ix, int* ns){
	if(text[*ix] == 'c'){
		while((text[*ix] != '\n') && (text[*ix] != 0)) (*ix)++;
		(*ix)++;
		return 0;
	}
	int ni = 0;
	while((text[*ix] != '\n') && (text[*ix] != 0)){
		if(text[*ix] > ' '){
			int n  = parseInt(text, ix);
			if(n != 0){
				ns[ni] = n;
				ni++;
			}else{
				(*ix)++;
				return ni;
			}
			(*ix)--;
		}
		(*ix)++;
	}
	(*ix)++;
	return ni;
}


Instance parseInstance(char* text, int size){
	Instance ret;
	int*  ns = alloca(sizeof(int) * 128);
	int   ix = 0;
	int   ni = 0;
	int head = 1;
	while(head){
		ni  += parseLine(text, &ix, ns);
		head = !ni;
	}
	if((ni == 4) && (ns[0] == 64) && (ns[1] == 5774)){
		ret.clausect  = ns[3];
		ret.literalct = ns[2];
	}else{
		error:
		ret.clausect  = -1;
		ret.literalct = -1;
		return ret;
	}
	
	ret.litcap    = ((ret.clausect * 8) > (ret.literalct * 8))? (ret.clausect * 8) : (ret.literalct * 8);
	ret.clauses   = malloc(sizeof(Clause) * ret.clausect);
	ret.literals  = malloc(sizeof(int   ) * ret.litcap);		// This should be enough, hopefully
	ret.litsize   = 0;
	ret.clausecap = ret.clausect;
	
	int lits   = 0;
	int clause = 0;
	while(ix < size){
		int k = parseLine(text, &ix, &ret.literals[ni]);
		if(k){
			if(clause >= ret.clausect) goto error;
			ret.clauses[clause] = (Clause){ni, k};
			clause++;
		}
		ni   += k;
	}
	ret.litsize = ni-4;
	
	return ret;
}

