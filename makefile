CC=gcc
FLAGS=-lm -lraylib -ggdb

.DEFAULT_GOAL := editor

EDITOR_SOURCE=src/tiledio.c src/tiledmap.c src/editor.c src/kdtree.c src/tiled.c src/tiledio.h src/tiledmap.h src/kdtree.h src/tiled.h
editor: ${EDITOR_SOURCE}
	${CC} ${EDITOR_SOURCE} -o editor ${FLAGS}

