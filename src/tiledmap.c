#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "tiledmap.h"

const int endian = 1;
#define is_bigendian() ( (*(char*)&endian) == 0 )

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

void buildChunkTree(TiledMap *tiledMap) {
    // initialise chunk tree
    tiledMap->chunkTree = NULL;

    size_t chunkSizeBytes = tiledMap->chunkWidth * tiledMap->chunkHeight;
    // calculate how much space there is until the end of the file
    // TODO save the number of chunks total to avoid having to jump around to find referenceso
    // or just keep reading chunks until eof or other descriptor
    long chunksStart = ftell(tiledMap->file);
    fseek(tiledMap->file, 0, SEEK_END);
    long totalChunksSize = ftell(tiledMap->file) - chunksStart;

    fseek(tiledMap->file, chunksStart, SEEK_SET);

    for (int n = 0; n < totalChunksSize; n += chunkSizeBytes + 8) {
        int x, y;
        readb((char *)&x, 4, tiledMap->file);
        readb((char *)&y, 4, tiledMap->file);
        long pointer = ftell(tiledMap->file);

        CachedChunk * cached = malloc(sizeof(CachedChunk));
        cached->filePos = pointer;
        cached->chunk = NULL;

        kdtree_insert(&tiledMap->chunkTree, x, y, (char *) cached);
        fseek(tiledMap->file, chunkSizeBytes, SEEK_CUR);
    }
}

TiledMap openTiledMap(char * filename) {
    TiledMap tiledMap;

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
    
    tiledMap.tileCount = tiledMap.atlasSize[0]*tiledMap.atlasSize[1] + 1;

    buildChunkTree(&tiledMap);
    return tiledMap;
}

//! save chunk to file
void commitChunk(TiledMap * tiledMap, CachedChunk * cached) {
    size_t chunkSizeBytes = tiledMap->chunkWidth * tiledMap->chunkHeight;

    fseek(tiledMap->file, cached->filePos, SEEK_SET);
    fwrite(cached->chunk, 1, chunkSizeBytes, tiledMap->file);
}

void unloadChunk(TiledMap * tiledMap, CachedChunk * cached) {
    // commit a chunk before unloading
    commitChunk(tiledMap, cached);

    // free memory
    free(cached->chunk);
    cached->chunk = NULL;
    
}

//! load a chunk into the cache and return it 
CachedChunk * loadChunk(TiledMap * tiledMap, int x, int y) {
    // TODO add caching for this
    size_t chunkSizeBytes = tiledMap->chunkWidth * tiledMap->chunkHeight;


    CachedChunk *cached = (CachedChunk * ) kdtree_search(tiledMap->chunkTree, x, y);
    // if this chunk is not indexed, return NULL
    if (cached == NULL)
        return NULL;

    // if this chunk is not loaded into memory, load it
    if (cached->chunk == NULL) {
        cached->chunk = malloc(chunkSizeBytes);
        fseek(tiledMap->file, cached->filePos, SEEK_SET);
        fread(cached->chunk, 1, chunkSizeBytes, tiledMap->file);
        printf("loading chunk %d,%d, from %ld\n", x, y, cached->filePos);
    }

    return cached;
}

CachedChunk * createChunk(TiledMap *tiledMap, int x, int y, Chunk chunk) {
    size_t chunkSizeBytes = tiledMap->chunkWidth * tiledMap->chunkHeight;

    fseek(tiledMap->file, 0, SEEK_END);

    // calculate position before writing
    long pos = ftell(tiledMap->file) + 8; 
    CachedChunk *cached = malloc(sizeof(CachedChunk));
    cached->filePos = pos;
    cached->chunk = chunk;

    kdtree_insert(&tiledMap->chunkTree, x, y, (char *) cached);

    int chunkx, chunky;
    writeb((char *) &x, 4, tiledMap->file);
    writeb((char *) &y, 4, tiledMap->file);
    fwrite(chunk, 1, chunkSizeBytes, tiledMap->file);

    return cached;
}

CachedChunk * createEmptyChunk(TiledMap * tiledMap, int x, int y) {
    Chunk chunk = calloc(tiledMap->chunkWidth*tiledMap->chunkHeight, 1);
    return createChunk(tiledMap, x, y, chunk);
}

Tile getChunkedTile(TiledMap *tiledMap, int x, int y) {
    // TODO put this calculation in function
    int inChunkX = x % tiledMap->chunkWidth;
    int inChunkY = y % tiledMap->chunkHeight;
    int chunkX = (x - inChunkX) / tiledMap->chunkWidth;
    int chunkY = (y - inChunkY) / tiledMap->chunkHeight;

    CachedChunk * cached = loadChunk(tiledMap, chunkX, chunkY);
    if (cached == NULL)
        return 0;

    if (cached->chunk == NULL)
        return 0;

    Tile v = cached->chunk[inChunkY * tiledMap->chunkWidth + inChunkX];
    return v;
}

Tile setChunkedTile(TiledMap * tiledMap, int x, int y, Tile value) {
    int inChunkX = x % tiledMap->chunkWidth;
    int inChunkY = y % tiledMap->chunkHeight;
    int chunkX = (x - inChunkX) / tiledMap->chunkWidth;
    int chunkY = (y - inChunkY) / tiledMap->chunkHeight;

    CachedChunk * cached = loadChunk(tiledMap, chunkX, chunkY);
    if (cached == NULL)
        cached = createEmptyChunk(tiledMap, chunkX, chunkY);

    cached->chunk[inChunkY * tiledMap->chunkWidth + inChunkX] = value;

    // TODO do this when unloading
    //commitChunk(tiledMap, cached);
    return value;
}


void writeTiledMapHeader(TiledMap tiledMap) {
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

TiledMap openNewTiledMap(char * filename, Image atlas, int tileSize, int chunkWidth, int chunkHeight, int width, int height) {
    TiledMap tiledMap;
    tiledMap.chunkWidth = chunkWidth;
    tiledMap.chunkHeight = chunkHeight;

    tiledMap.tileSize = tileSize;

    tiledMap.atlasSize[0] = atlas.width / tileSize;
    tiledMap.atlasSize[1] = atlas.height / tileSize;

    tiledMap.atlasData = LoadImageColors(atlas);
    tiledMap.tileCount = tiledMap.atlasSize[0]*tiledMap.atlasSize[1] + 1;

    tiledMap.chunkTree = NULL;

    if (!(tiledMap.file = fopen(filename, "wb"))) {
        fprintf(stderr, "Failed to load %s\n", filename);
    }

    writeTiledMapHeader(tiledMap);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            Chunk chunk = calloc(chunkWidth*chunkHeight, 1);
            createChunk(&tiledMap, x, y,  chunk);
        }
    }
    
    // reopen the file in read+write mode
    fclose(tiledMap.file);

    if (!(tiledMap.file = fopen(filename, "r+b"))) {
        fprintf(stderr, "Failed to load %s\n", filename);
    }

    return tiledMap;
}

void unloadChunks(TiledMap *tiledMap, kdtree_t * root) {
    if (root == NULL)
        return;

    unloadChunks(tiledMap, root->left);
    unloadChunks(tiledMap, root->right);
    unloadChunk(tiledMap, (CachedChunk *)root->value);
}

void closeTiledMap(TiledMap *tiledMap) {
    unloadChunks(tiledMap, tiledMap->chunkTree);
    kdtree_free(&tiledMap->chunkTree);
    UnloadImageColors(tiledMap->atlasData);
    fclose(tiledMap->file);
}

