#-*- Makefile -*-

all: minesweeper.o helpers.o GUI.o
	gcc minesweeper.o helpers.o GUI.o -lSDL2  -o minesweeper

minesweeper.o: minesweeper.c minesweeper.h helpers.h GUI.h
	gcc minesweeper.c -c -o minesweeper.o

helpers.o: helpers.c helpers.h
	gcc helpers.c -c -o helpers.o

GUI.o: GUI.c GUI.h minesweeper.h
	gcc GUI.c -c -o GUI.o 

clean:
	rm *.o minesweeper
