#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wchar.h>

#include "chunkedtiledmap.h"

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

void buildChunkTree(ChunkedTiledMap *tiledMap) {
    int noChunks;

    // 4 bytes for number of chunks
    readb((char *)&noChunks, 4, tiledMap->file);
    size_t chunkSizeBytes = tiledMap->chunkWidth * tiledMap->chunkHeight;
    for (int n = 0; n < noChunks; n++) {
        int x, y;
        readb((char *)&x, 4, tiledMap->file);
        readb((char *)&y, 4, tiledMap->file);
        long pointer = ftell(tiledMap->file);
        // TODO casting to pointer here is maybe unsafe
        kdtree_insert(&tiledMap->chunkTree, x, y, (char *) pointer);
        fseek(tiledMap->file, chunkSizeBytes, SEEK_CUR);
    }
}

ChunkedTiledMap openTiledMap(char * filename) {
    ChunkedTiledMap tiledMap;

    if (!(tiledMap.file = fopen(filename, "r+b"))) {
        fprintf(stderr, "Failed to load %s\n", filename);
    }

    FILE * file = tiledMap.file;
    
    // skip header 
    fseek(file, 10, SEEK_CUR);
    // 4 bytes for int width
    readb((char *)&tiledMap.chunkWidth, 4, file);
    // 4 bytes for int height
    readb((char *)&tiledMap.chunkHeight, 4, file);

    // read the pixel size of each tile
    readb((char *)&tiledMap.tileSize, 4, file);

    // read the atlas size
    readb((char *)&tiledMap.atlasSize[0], 4, file);
    readb((char *)&tiledMap.atlasSize[1], 4, file);

    // read the atlas itself
    size_t atlasSizeBytes = tiledMap.atlasSize[0]*tiledMap.tileSize*tiledMap.atlasSize[1]*tiledMap.tileSize*4;
    tiledMap.atlasData = malloc(atlasSizeBytes);
    fread(tiledMap.atlasData, atlasSizeBytes, (size_t) 1, file);
    
    buildChunkTree(&tiledMap);
    return tiledMap;
}

char* loadChunk(ChunkedTiledMap tiledMap, int x, int y) {
    // TODO add caching for this
    size_t chunkSizeBytes = tiledMap.chunkWidth * tiledMap.chunkHeight;
    char * chunk = malloc(chunkSizeBytes);

    long pos = (long) kdtree_search(tiledMap.chunkTree, x, y);
    fseek(tiledMap.file, pos, SEEK_SET);
    fread(chunk, 1, chunkSizeBytes, tiledMap.file);
    return chunk;
}

void appendChunk(ChunkedTiledMap *tiledMap, int x, int y, char * chunk) {
    // TODO does this actually need tiledMap as a pointer
    size_t chunkSizeBytes = tiledMap->chunkWidth * tiledMap->chunkHeight;

    fseek(tiledMap->file, 0, SEEK_END);

    // calculate position before writing
    long pos = ftell(tiledMap->file) + 8; 
    kdtree_insert(&tiledMap->chunkTree, x, y, chunk);

    writeb((char *) &x, 4, tiledMap->file);
    writeb((char *) &y, 4, tiledMap->file);
    fwrite(chunk, 1, chunkSizeBytes, tiledMap->file);
}


void writeTiledMapHeader(ChunkedTiledMap tiledMap) {
    FILE * file = tiledMap.file;
    size_t atlasSizeBytes = tiledMap.atlasSize[0]*tiledMap.tileSize*tiledMap.atlasSize[1]*tiledMap.tileSize*4;

    // write header information from the start of the file
    fseek(file, 0, SEEK_SET);
    fwrite("TILEFILEv3", 10, 1, file);

    writeb((char *) &tiledMap.chunkWidth, 4, file);
    writeb((char *) &tiledMap.chunkHeight, 4, file);

    writeb((char *) &tiledMap.tileSize, 4, file);
    writeb((char *) &tiledMap.atlasSize[0], 4, file);
    writeb((char *) &tiledMap.atlasSize[1], 4, file);

    fwrite(tiledMap.atlasData, 1, atlasSizeBytes, file);
    // since chunks are already directly written here, do not write anything else
    // TODO when caching, commit everything left in cache here
}

ChunkedTiledMap newTiledMap(char * filename, Image atlas, int tileSize, int chunkWidth, int chunkHeight, int width, int height) {
    ChunkedTiledMap tiledMap;
    tiledMap.chunkWidth = chunkWidth;
    tiledMap.chunkHeight = chunkHeight;

    tiledMap.tileSize = tileSize;

    tiledMap.atlasSize[0] = atlas.width / tileSize;
    tiledMap.atlasSize[1] = atlas.height / tileSize;

    tiledMap.atlasData = LoadImageColors(atlas);

    if (!(tiledMap.file = fopen(filename, "r+b"))) {
        fprintf(stderr, "Failed to load %s\n", filename);
    }

    writeTiledMapHeader(tiledMap);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            char * chunk = calloc(chunkWidth*chunkHeight, 1);
            appendChunk(&tiledMap, x, y,  chunk);
        }
    }

    return tiledMap;
}

void closeTiledMap(ChunkedTiledMap tiledMap) {
    kdtree_free(&tiledMap.chunkTree);
    UnloadImageColors(tiledMap.atlasData);
    fclose(tiledMap.file);
}



