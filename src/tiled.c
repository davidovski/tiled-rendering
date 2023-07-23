#include <raylib.h>
#include <stdio.h>

#include "tiled.h"

int alpha =0;

void updateCamera(Vector2 *offset, float *zoom) {
    if (IsKeyDown(KEY_UP)) offset->y += 16.0f / *zoom;
    if (IsKeyDown(KEY_DOWN)) offset->y -=  16.0f/ *zoom;
    if (IsKeyDown(KEY_RIGHT)) offset->x -= 16.0f / *zoom;
    if (IsKeyDown(KEY_LEFT)) offset->x += 16.0f / *zoom;

    if (IsKeyDown(KEY_W)) *zoom += *zoom * 0.01f;
    if (IsKeyDown(KEY_S)) *zoom -= *zoom * 0.01f;
}

void updateTiledCamera(Tiled *tiled) {
    updateCamera(&tiled->offset, &tiled->zoom);
    alpha++;
}


void initTiledShader(Tiled *tiled) {
    tiled->shader = LoadShader(0, "tiled.glsl");

    tiled->offsetLoc = GetShaderLocation(tiled->shader, "offset");
    tiled->zoomLoc = GetShaderLocation(tiled->shader, "zoom");

    tiled->atlasSizeLoc = GetShaderLocation(tiled->shader, "atlasSize");
    tiled->mapSizeLoc = GetShaderLocation(tiled->shader, "mapSize");

    tiled->atlasTextureLoc = GetShaderLocation(tiled->shader, "atlasTexture");
    tiled->tilemapTextureLoc = GetShaderLocation(tiled->shader, "tilemapTexture");
}

Vector2 translateTiledPosition(Tiled tiled, Vector2 screenPos) {
    return (Vector2) {
        screenPos.x / tiled.zoom - tiled.offset.x,
        screenPos.y / tiled.zoom - tiled.offset.y
    };
}

Vector2 translateTiledScreenPosition(Tiled tiled, Vector2 tiledPos) {
    return (Vector2) {
        (tiledPos.x + tiled.offset.x) * tiled.zoom,
        (tiledPos.y + tiled.offset.y) * tiled.zoom
    };
}

void redrawTile(Tiled tiled, int x, int y) {
    BeginTextureMode(tiled.tilemapTexture);
    unsigned char v = getChunkedTile(&tiled.tiledMap, x, y);
    Color c = (Color){ 
        v, 0, 0, 255 
    };
    DrawPixel(x, tiled.mapSize[1] - y - 1, c);
    EndTextureMode();
}

void redrawTiledMap(Tiled tiled) {
    BeginTextureMode(tiled.tilemapTexture);
    for (int y = 0; y < tiled.mapSize[1]; y++) {
        for (int x = 0; x < tiled.mapSize[0]; x++) {
            unsigned char v = getChunkedTile(&tiled.tiledMap, x, y);
            Color c = (Color){ 
                v, 0, 0, 255 
            };
            DrawPixel(x, tiled.mapSize[1] - y - 1, c);
        }
    }
    EndTextureMode();

}


Tiled initTiled(TiledMap tiledMap) {
    Tiled tiled;
    tiled.tiledMap = tiledMap;

    tiled.offset = (Vector2) {0, 0};
    tiled.zoom = 64;

    // TODO mapSize is obsolete, should be visible map size
    tiled.mapSize[0] = tiledMap.chunkWidth * 20;
    tiled.mapSize[1] = tiledMap.chunkHeight * 20;
    tiled.targetTexture = LoadRenderTexture(1, 1);
    tiled.tilemapTexture = LoadRenderTexture(tiled.mapSize[0], tiled.mapSize[1]);

    tiled.atlasSize[0] = tiledMap.atlasSize[0];
    tiled.atlasSize[1] = tiledMap.atlasSize[1];

    textureFromPixels(&tiled.atlasTexture,
            tiledMap.atlasData,
            tiledMap.atlasSize[0] * tiledMap.tileSize,
            tiledMap.atlasSize[1] * tiledMap.tileSize);


    redrawTiledMap(tiled);
    initTiledShader(&tiled);
    return tiled;
}

void setTiledShaderUniforms(Tiled tiled) {
    SetShaderValue(tiled.shader, tiled.offsetLoc, &tiled.offset, SHADER_UNIFORM_VEC2);
    SetShaderValue(tiled.shader, tiled.zoomLoc, &tiled.zoom, SHADER_UNIFORM_FLOAT);

    SetShaderValue(tiled.shader, tiled.atlasSizeLoc, &tiled.atlasSize, SHADER_UNIFORM_IVEC2);
    SetShaderValue(tiled.shader, tiled.mapSizeLoc, &tiled.mapSize, SHADER_UNIFORM_IVEC2);

    SetShaderValueTexture(tiled.shader, tiled.atlasTextureLoc, tiled.atlasTexture);
    SetShaderValueTexture(tiled.shader, tiled.tilemapTextureLoc, tiled.tilemapTexture.texture);
}

void unloadTiled(Tiled *tiled) {
    UnloadShader(tiled->shader);
    UnloadTexture(tiled->atlasTexture);
    UnloadRenderTexture(tiled->tilemapTexture);
    UnloadRenderTexture(tiled->targetTexture);
}
void drawTiled(Tiled *tiled) {
    //renderTiledMapTexture(tiled->tiledMap, tiled->tilemapTexture);
    BeginShaderMode(tiled->shader);
    setTiledShaderUniforms(*tiled);

    DrawTextureRec(tiled->targetTexture.texture,
            (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
            (Vector2){0, 0},
            WHITE);
    EndShaderMode();
    //DrawTextureRec(tiled->tilemapTexture.texture,
            //(Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
            //(Vector2){0, 0},
            //WHITE);
}
