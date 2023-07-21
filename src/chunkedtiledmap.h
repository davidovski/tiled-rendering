#include <raylib.h>
#include "kdtree.h"

typedef struct ChunkedTiledMap {
    FILE * file;
    int chunkWidth;
    int chunkHeight;
    int tileSize;
    int atlasSize[2];
    int tileCount;
    Color * atlasData;
    kdtree_t * chunkTree;
} ChunkedTiledMap;

void textureFromPixels(Texture2D *texOut, Color *pixels, int width, int height);
ChunkedTiledMap openTiledMap(char * filename);
char * loadChunk(ChunkedTiledMap tiledMap, int x, int y);
char getChunkedTile(ChunkedTiledMap tiledMap, int x, int y);
char setChunkedTile(ChunkedTiledMap * tiledMap, int x, int y, char value);
char * appendChunk(ChunkedTiledMap * tiledMap, int x, int y, char * chunk);
void writeTiledMapHeader(ChunkedTiledMap tiledMap);
ChunkedTiledMap openNewTiledMap(char * filename, Image atlas, int tileSize, int chunkWidth, int chunkHeight, int width, int height);
void closeTiledMap(ChunkedTiledMap tiledMap);

