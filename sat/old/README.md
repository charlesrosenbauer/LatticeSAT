# Ophanim SMT solver

This is the SMT/SAT solver for Bzo.

There are currently three main levels of representation:

* Bytecode
	* opcodes like ADD, SUB, MUL, DIV, PCT, etc.
	* oriented toward loop-free code, though loops may eventually be handled somewhat
	* not sure how to handle memory quite yet
	* translates into gates, with constraints
* Gate
	* logic gates - and, or, xor, eq, nand, nor, xnor, not
	* circuit simplification
	* translates into SAT
* SAT
	* vectorized bit hacking on SAT subinstances
	* parallelism where possible
