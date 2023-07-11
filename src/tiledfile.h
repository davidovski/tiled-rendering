#include <raylib.h>

typedef struct TiledMap {
    int width;
    int height;
    char * tilelayout;
    int tilesize;
    int atlasSize[2];
    Color * atlasData;
} TiledMap;

void textureFromPixels(Texture2D *texOut, Color *pixels, int width, int height);

void renderTilemapTexture(Texture2D *texOut, TiledMap tiledMap);

TiledMap loadTiledMap(char * filename);
