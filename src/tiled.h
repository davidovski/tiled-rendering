#include <raylib.h>

typedef struct TiledUniforms {
    float zoom;
    Vector2 offset;

    int atlasSize[2];
    int mapSize[2];

    Texture2D atlasTexture;
    Texture2D tilemapTexture;


    int zoomLoc;
    int offsetLoc;

    int atlasSizeLoc;
    int mapSizeLoc;

    int atlasTextureLoc;
    int tilemapTextureLoc;

} TiledUniforms;
