# makefile for mines project

all:
	g++ -o./bin/mines -g -Wall src/mines.cpp src/mtrand.cpp -lSDL -lSDLmain
