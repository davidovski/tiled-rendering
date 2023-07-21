CC=gcc
FLAGS=-lm -lraylib -ggdb

.DEFAULT_GOAL := editor

EDITOR_SOURCE=src/chunkedtiledmap.c src/editor.c src/kdtree.c src/tiled.c 
editor: ${EDITOR_SOURCE}
	${CC} ${EDITOR_SOURCE} -o editor ${FLAGS}

