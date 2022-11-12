all:
	clang  -lSDL -O3     -D _RELEASE sat/*.c -o satc

safe:
	clang  -lSDL -O2     -D _RELEASE sat/*.c -o satc

debug:
	clang -lSDL -g -pg -Og -O0 -D _DEBUG   sat/*.c -o satc

