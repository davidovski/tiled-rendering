CC=gcc
FLAGS=-lm -lraylib -ggdb

.DEFAULT_GOAL := build

install: tiled
	cp tiled ${PREFIX}/bin/

build: src/*.c src/*.h
	${CC} src/*.c -o tiled ${FLAGS} 

editor: src/*.c src/editor.c
	${CC} src/*.c -o editor ${FLAGS}

clean: tiled
	rm tiled

