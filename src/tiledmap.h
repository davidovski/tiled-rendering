#include <raylib.h>
#include "kdtree.h"

#define CHUNK_CACHE_SIZE 32

typedef unsigned char Tile;
typedef Tile * Chunk;

typedef struct CachedChunk {
    long filePos;
    Chunk chunk;
} CachedChunk;

typedef struct ChunkedTiledMap {
    FILE * file;
    int chunkWidth;
    int chunkHeight;
    int tileSize;
    int atlasSize[2];
    int tileCount;
    Color * atlasData;
    kdtree_t * chunkTree;
} TiledMap;

void textureFromPixels(Texture2D *texOut, Color *pixels, int width, int height);
TiledMap openTiledMap(char * filename);
CachedChunk * loadChunk(TiledMap *tiledMap, int x, int y);
Tile getChunkedTile(TiledMap *tiledMap, int x, int y);
Tile setChunkedTile(TiledMap * tiledMap, int x, int y, Tile value);
CachedChunk * createChunk(TiledMap * tiledMap, int x, int y, Chunk chunk);
void writeTiledMapHeader(TiledMap tiledMap);
TiledMap openNewTiledMap(char * filename, Image atlas, int tileSize, int chunkWidth, int chunkHeight, int width, int height);
void closeTiledMap(TiledMap * tiledMap);

