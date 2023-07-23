#include <raylib.h>

#include "tiledmap.h"

#define SCREEN_W 1280
#define SCREEN_H 720

#define RENDER_DISTANCE 16

typedef struct Tiled {
    TiledMap tiledMap;
    float zoom;
    
    // main viewport offset
    Vector2 offset;

    // offset passed to shader for rendering
    Vector2 renderOffset;
    // offset used to calculate which chunks to render
    int chunkOffset[2];

    int atlasSize[2];
    int renderArea[2];

    Texture2D atlasTexture;

    RenderTexture2D tilemapTexture;
    RenderTexture2D targetTexture;
    Shader shader; 

    int zoomLoc;
    int offsetLoc;

    int atlasSizeLoc;
    int renderAreaLoc;

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
void redrawTile(Tiled tiled, int x, int y);

