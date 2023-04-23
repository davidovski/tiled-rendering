PROG=tiled
CC=gcc
FLAGS=-lm -lraylib

.DEFAULT_GOAL := build

install: ${PROG}
	cp ${PROG} ${PREFIX}/bin/

build: ${PROG}.c
	${CC} ${PROG}.c -o ${PROG} ${FLAGS}

clean: ${PROG}
	rm ${PROG}

