#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tiledmap.h"

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
}

//! read rgba image from file
void readrgba(Texture2D *loc, int width, int height, FILE *file) {
    Color *pixels = malloc(width*height*4);
    fread(pixels, (size_t) width*height*4, (size_t) 1, file);
    textureFromPixels(loc, pixels, width, height);
}



//! write a big endian bytes from file
int writeb(char * in, size_t noBytes, FILE * file) {
    if (!is_bigendian()) {
        int tmp;
        // reverse byte order
        for(int i = 0; i < noBytes/2; i++) {
            tmp = in[i];
            in[i] = in[noBytes-i-1];
            in[noBytes-i-1] = tmp;
        }
        
    }

    return fwrite(in, (size_t)1, (size_t) noBytes, file);
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

char getTiledMapTile(TiledMap tiledMap, int pos[2]) {
    return tiledMap.tilelayout[pos[1]*tiledMap.width + pos[0]];
}

void setTiledMapTile(TiledMap tiledMap, int pos[2], char tile) {
    tiledMap.tilelayout[pos[1]*tiledMap.width + pos[0]] = tile;
}

//! load tilemap data from file
TiledMap loadTiledMap(char * filename) {
    TiledMap tiledMap;
    FILE * file;

    if (!(file = fopen(filename, "rb"))) {
        fprintf(stderr, "Failed to load %s\n", filename);
        return tiledMap;
    }

    // skip header 
    fseek(file, 10, SEEK_CUR);
    // 4 bytes for int width
    readb((char *)&tiledMap.width, 4, file);
    // 4 bytes for int height
    readb((char *)&tiledMap.height, 4, file);

    size_t layoutSize = tiledMap.width*tiledMap.height;
    tiledMap.tilelayout = malloc(layoutSize);
    fread(tiledMap.tilelayout, layoutSize, 1, file);

    // read the pixel size of each tile
    readb((char *)&tiledMap.tileSize, 4, file);

    // read the atlas size
    readb((char *)&tiledMap.atlasSize[0], 4, file);
    readb((char *)&tiledMap.atlasSize[1], 4, file);

    // read the atlas itself
    size_t atlasSizeBytes = tiledMap.atlasSize[0]*tiledMap.tileSize*tiledMap.atlasSize[1]*tiledMap.tileSize*4;
    tiledMap.atlasData = malloc(atlasSizeBytes);
    fread(tiledMap.atlasData, atlasSizeBytes, (size_t) 1, file);

    tiledMap.tileCount = tiledMap.atlasSize[0]*tiledMap.atlasSize[1] + 1;

    fclose(file);
    return tiledMap;
}

TiledMap openNewTiledMap(Image atlas, int tileSize, int width, int height) {
    TiledMap tiledMap;
    tiledMap.width = width;
    tiledMap.height = height;
    tiledMap.tilelayout = malloc(width * height);

    tiledMap.tileSize = tileSize;

    tiledMap.atlasSize[0] = atlas.width / tileSize;
    tiledMap.atlasSize[1] = atlas.height / tileSize;

    tiledMap.atlasData = LoadImageColors(atlas);

    return tiledMap;
}

void saveTiledMap(char * filename, TiledMap tiledMap) {
    FILE * file;

    if (!(file = fopen(filename, "wb"))) {
        fprintf(stderr, "Failed to load %s\n", filename);
        return;
    }
    size_t layoutSize = tiledMap.width*tiledMap.height;
    size_t atlasSizeBytes = tiledMap.atlasSize[0]*tiledMap.tileSize*tiledMap.atlasSize[1]*tiledMap.tileSize*4;

    fwrite("TILEFILEv2", 10, 1, file);

    writeb((char *) &tiledMap.width, 4, file);
    writeb((char *) &tiledMap.height, 4, file);
    
    fwrite(tiledMap.tilelayout, 1, layoutSize, file);

    writeb((char *) &tiledMap.tileSize, 4, file);
    writeb((char *) &tiledMap.atlasSize[0], 4, file);
    writeb((char *) &tiledMap.atlasSize[1], 4, file);

    fwrite(tiledMap.atlasData, 1, atlasSizeBytes, file);

    fclose(file);
    fprintf(stderr, "Written tiledfiled to %s\n", filename);
}
