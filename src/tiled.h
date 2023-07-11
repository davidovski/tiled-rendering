#include <raylib.h>

#include "tiledfile.h"

#define SCREEN_W 1280
#define SCREEN_H 720

typedef struct Tiled {
    TiledMap tiledMap;
    float zoom;
    Vector2 offset;

    int atlasSize[2];
    int mapSize[2];

    Texture2D atlasTexture;

    RenderTexture2D tilemapTexture;
    RenderTexture2D targetTexture;
    Shader shader; 

    int zoomLoc;
    int offsetLoc;

    int atlasSizeLoc;
    int mapSizeLoc;

    int atlasTextureLoc;
    int tilemapTextureLoc;

} Tiled;

void updateTiledCamera(Tiled *tiled);
Vector2 translateTiledPosition(Tiled tiled, Vector2 screenPos);
Vector2 translateTiledScreenPosition(Tiled tiled, Vector2 tiledPos);
Tiled initTiled(TiledMap tiledMap);
void drawTiled(Tiled *tiled);
void unloadTiled(Tiled *tiled);
void redrawTiledMap(Tiled tiled);

