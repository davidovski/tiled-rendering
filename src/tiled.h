#include <raylib.h>

#include "tiledfile.h"

typedef struct Tiled {
    float zoom;
    Vector2 offset;

    int atlasSize[2];
    int mapSize[2];

    Texture2D atlasTexture;
    Texture2D tilemapTexture;

    RenderTexture2D targetTexture;
    Shader shader; 

    int zoomLoc;
    int offsetLoc;

    int atlasSizeLoc;
    int mapSizeLoc;

    int atlasTextureLoc;
    int tilemapTextureLoc;

} Tiled;

void initTiled(Tiled *tiled, TiledMap tiledMap);
void drawTiled(Tiled *tiled);
void unloadTiled(Tiled *tiled);
