#!/usr/bin/env python
import sys

outfile = "map.tiles"

WIDTH = 500
HEIGHT = 100
# number of bytes each tile needs to represent
TILEBYTES = 25

HEADER = "TILEFILEv1"

with open(outfile, "wb") as file:
    file.write(bytes(HEADER, "ascii"))
    file.write(WIDTH.to_bytes(4, 'big'));
    file.write(HEIGHT.to_bytes(4, 'big'));
    file.write(TILEBYTES.to_bytes(4, 'big'));

    for y in range(HEIGHT):
        for x in range(WIDTH):
            index = (x + y) % 5
            file.write(index.to_bytes(TILEBYTES, 'big'));

