all:
	g++ -Isrc/Include -Lsrc/lib -o uler src/uler.cpp -lmingw32 -lSDL2main -lSDL2