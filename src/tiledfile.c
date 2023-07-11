#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>

#include "tiledfile.h"

const int i = 1;
#define is_bigendian() ( (*(char*)&i) == 0 )

void textureFromPixels(Texture2D *texOut, Color *pixels, int width, int height) {
    Image checkedIm = {
        .data = pixels,
        .width = width,
        .height = height,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    };

    *texOut = LoadTextureFromImage(checkedIm);
    UnloadImage(checkedIm);
}

void renderTilemapTexture(Texture2D *texOut, TiledMap tiledMap) {
    Color *pixels = (Color*) malloc(tiledMap.width * tiledMap.height * sizeof(Color));

    for (int i = 0; i < tiledMap.width*tiledMap.height; i++) {
        pixels[i] = (Color){ tiledMap.tilelayout[i], 0, 0, 0 };
    }

    textureFromPixels(texOut, pixels, tiledMap.width, tiledMap.height);
}

//! read rgba image from file
void readrgba(Texture2D *loc, int width, int height, FILE *file) {
    Color *pixels = malloc(width*height*4);
    fread(pixels, (size_t) width*height*4, (size_t) 1, file);
    textureFromPixels(loc, pixels, width, height);
}


//! read a big endian bytes from file
int readb(char * out, size_t noBytes, FILE * file) {
    if (!fread(out, (size_t)1, (size_t) noBytes, file))
        return 1;

    if (is_bigendian()) 
        return 0;

    int tmp;
    // reverse byte order
    for(int i = 0; i < noBytes/2; i++) {
        tmp = out[i];
        out[i] = out[noBytes-i-1];
        out[noBytes-i-1] = tmp;
    }

    return 0;
}

//! load tilemap data from file
TiledMap loadTiledMap(char * filename) {
    TiledMap tiledmap;
    FILE * file;

    if (!(file = fopen(filename, "rb"))) {
        fprintf(stderr, "Failed to load %s\n", filename);
        return tiledmap;
    }

    // skip header 
    fseek(file, 10, SEEK_CUR);
    // 4 bytes for int width
    readb((char *)&tiledmap.width, 4, file);
    // 4 bytes for int height
    readb((char *)&tiledmap.height, 4, file);

    size_t layoutSize = tiledmap.width*tiledmap.height;
    tiledmap.tilelayout = malloc(layoutSize);
    fread(tiledmap.tilelayout, layoutSize, 1, file);

    // read the pixel size of each tile
    readb((char *)&tiledmap.tilesize, 4, file);

    // read the atlas size
    readb((char *)&tiledmap.atlasSize[0], 4, file);
    readb((char *)&tiledmap.atlasSize[1], 4, file);

    // read the atlas itself
    size_t atlasSizeBytes = tiledmap.atlasSize[0]*tiledmap.tilesize*tiledmap.atlasSize[1]*tiledmap.tilesize*4;
    tiledmap.atlasData = malloc(atlasSizeBytes);
    fread(tiledmap.atlasData, atlasSizeBytes, (size_t) 1, file);

    fclose(file);
    return tiledmap;
}
