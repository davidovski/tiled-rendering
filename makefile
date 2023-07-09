CC=gcc
FLAGS=-lm -lraylib

.DEFAULT_GOAL := build

install: tiled
	cp tiled ${PREFIX}/bin/

build: tiled.c tiledfile.c tiledfile.h
	${CC} tiled.c tiledfile.c -o tiled ${FLAGS} 

clean: tiled
	rm tiled

