#include <raylib.h>
#include "kdtree.h"

const int i = 1;
#define is_bigendian() ( (*(char*)&i) == 0 )

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
