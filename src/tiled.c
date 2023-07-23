#include <raylib.h>
#include <stdio.h>

#include "tiled.h"

int alpha =0;

void updateCamera(Tiled * tiled) {
    if (IsKeyDown(KEY_UP)) tiled->offset.y += 16.0f / tiled->zoom;
    if (IsKeyDown(KEY_DOWN)) tiled->offset.y -=  16.0f / tiled->zoom;
    if (IsKeyDown(KEY_RIGHT)) tiled->offset.x -= 16.0f / tiled->zoom;
    if (IsKeyDown(KEY_LEFT)) tiled->offset.x += 16.0f / tiled->zoom;

    if (IsKeyDown(KEY_W)) tiled->zoom += tiled->zoom * 0.01f;
    if (IsKeyDown(KEY_S)) tiled->zoom -= tiled->zoom * 0.01f;

    tiled->chunkOffset[2] = tiled->chunkOffset[0];
    tiled->chunkOffset[3] = tiled->chunkOffset[1];

    tiled->chunkOffset[0] = tiled->offset.x / tiled->tiledMap.chunkWidth;
    tiled->chunkOffset[1] = tiled->offset.y / tiled->tiledMap.chunkHeight;

    tiled->renderOffset.x = tiled->offset.x - tiled->chunkOffset[0]*tiled->tiledMap.chunkWidth;
    tiled->renderOffset.y = tiled->offset.y - tiled->chunkOffset[1]*tiled->tiledMap.chunkHeight;

    // TODO unload chunks when they are no longer visible
    if (tiled->chunkOffset[0] != tiled->chunkOffset[2]
        || tiled->chunkOffset[1] != tiled->chunkOffset[3])
        redrawTiledMap(*tiled);
}

void updateTiledCamera(Tiled *tiled) {
    updateCamera(tiled);
    alpha++;
}

void initTiledShader(Tiled *tiled) {
    tiled->shader = LoadShader(0, "tiled.glsl");

    tiled->offsetLoc = GetShaderLocation(tiled->shader, "offset");
    tiled->zoomLoc = GetShaderLocation(tiled->shader, "zoom");

    tiled->atlasSizeLoc = GetShaderLocation(tiled->shader, "atlasSize");
    tiled->renderAreaLoc = GetShaderLocation(tiled->shader, "renderArea");

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
    int areaX = x + tiled.chunkOffset[0]*tiled.tiledMap.chunkWidth;
    int areaY = tiled.renderArea[1] - y - tiled.chunkOffset[1]*tiled.tiledMap.chunkHeight - 1;

    Tile v = getChunkedTile(&tiled.tiledMap, x, y);

    BeginTextureMode(tiled.tilemapTexture);
    DrawPixel(areaX, areaY, (Color){ v, 0, 0, 255 });
    EndTextureMode();
}

void redrawTiledMap(Tiled tiled) {
    // TODO since we have to do this every time a chunk is loaded, maybe there is a better way
    int realX, realY;
    Tile v;

    BeginTextureMode(tiled.tilemapTexture);
    for (int y = 0; y < tiled.renderArea[1]; y++) {
        for (int x = 0; x < tiled.renderArea[0]; x++) {
            realX = x - tiled.chunkOffset[0]*tiled.tiledMap.chunkWidth;
            realY = y - tiled.chunkOffset[1]*tiled.tiledMap.chunkHeight;

            v = getChunkedTile(&tiled.tiledMap, realX, realY);

            DrawPixel(x, tiled.renderArea[1] - y - 1, (Color){ v, 0, 0, 255 });
        }
    }
    EndTextureMode();
}

Tiled initTiled(TiledMap tiledMap) {
    Tiled tiled;
    tiled.tiledMap = tiledMap;

    tiled.offset = (Vector2) {0, 0};
    tiled.zoom = 64;

    tiled.renderArea[0] = tiledMap.chunkWidth * RENDER_DISTANCE;
    tiled.renderArea[1] = tiledMap.chunkHeight * RENDER_DISTANCE;

    printf("Render area %dx%d\n", tiled.renderArea[0], tiled.renderArea[1]);

    tiled.targetTexture = LoadRenderTexture(1, 1);
    tiled.tilemapTexture = LoadRenderTexture(tiled.renderArea[0], tiled.renderArea[1]);

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
    SetShaderValue(tiled.shader, tiled.offsetLoc, &tiled.renderOffset, SHADER_UNIFORM_VEC2);
    SetShaderValue(tiled.shader, tiled.zoomLoc, &tiled.zoom, SHADER_UNIFORM_FLOAT);

    SetShaderValue(tiled.shader, tiled.atlasSizeLoc, &tiled.atlasSize, SHADER_UNIFORM_IVEC2);
    SetShaderValue(tiled.shader, tiled.renderAreaLoc, &tiled.renderArea, SHADER_UNIFORM_IVEC2);


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
