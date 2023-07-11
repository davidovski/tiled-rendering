#!/usr/bin/env python
import sys
from math import ceil, log
from PIL import Image

outfile = "map.tiles"

ATLASFILE = "atlas.png"

TILESIZE = 16
WIDTH = 64
HEIGHT = 64

# create atlas bytes
image = Image.open(ATLASFILE).convert("RGBA")


# check if atlas is suitable
if image.width % TILESIZE != 0 or image.height % TILESIZE != 0:
    print(f"Atlas image must be divisible by {TILESIZE}", file=sys.err)
    sys.exit(1)

# calculate number of possible tiles (adding 1 for the empty tile)
tilecount = (image.width * image.height) / (TILESIZE**2) + 1

atlas_width = image.width // TILESIZE
atlas_height = image.width // TILESIZE

# number of bytes each tile needs to represent
tilebytes = ceil(log(tilecount, 256))

HEADER = "TILEFILEv1"

with open(outfile, "wb") as file:
    file.write(bytes(HEADER, "ascii"))
    file.write(WIDTH.to_bytes(4, 'big'));
    file.write(HEIGHT.to_bytes(4, 'big'));
    #file.write(tilebytes.to_bytes(4, 'big'));


    for y in range(HEIGHT):
        for x in range(WIDTH):
            index = 0;
            file.write(index.to_bytes(tilebytes, 'big'));

    file.write(TILESIZE.to_bytes(4, 'big'));
    file.write(atlas_width.to_bytes(4, 'big'));
    file.write(atlas_height.to_bytes(4, 'big'));
    file.write(image.tobytes())
