# LatticeSAT

---

This is a new approach to SAT solving based roughly on lattice operations accelerated by vectorized bitwise operations, combined with something similar to the wave-function collapse algorithm.

I'd like to eventually support both SAT solving and SAT counting. SAT counting is of course harder, but I think the algorithmic approach I'm taking will at the very least make it much easier to place tight upper bounds on solution counts.

I'd like this SAT solver to also be useful for doing analysis on SAT problems, which can hopefully be combined with visualization tools.
