#include <raylib.h>

typedef struct TiledMap {
    int width;
    int height;
    char * tilelayout;
    int tileSize;
    int atlasSize[2];
    Color * atlasData;
} TiledMap;

void textureFromPixels(Texture2D *texOut, Color *pixels, int width, int height);

void renderTilemapTexture(Texture2D *texOut, TiledMap tiledMap);

void setTiledMapTile(TiledMap tiledMap, int pos[2], char tile);

char getTiledMapTile(TiledMap tiledMap, int pos[2]);

TiledMap loadTiledMap(char * filename);
