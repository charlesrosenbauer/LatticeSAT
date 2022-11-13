all:
	clang  -lSDL -O3     -D _RELEASE src/*.c -o satc

safe:
	clang  -lSDL -O2     -D _RELEASE src/*.c -o satc

debug:
	clang -lSDL -g -pg -Og -O0 -D _DEBUG   src/*.c -o satc

