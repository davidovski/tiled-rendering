CC=gcc
FLAGS=-lm -lraylib -ggdb

.DEFAULT_GOAL := editor

editor: src/*.c src/editor.c
	${CC} src/*.c -o editor ${FLAGS}

