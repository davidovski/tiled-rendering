CC=gcc
FLAGS=-lm -lraylib -ggdb

.DEFAULT_GOAL := build

install: tiled
	cp tiled ${PREFIX}/bin/

build: src/*.c src/*.h
	${CC} src/*.c -o tiled ${FLAGS} 

clean: tiled
	rm tiled

