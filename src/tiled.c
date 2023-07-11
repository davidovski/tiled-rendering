#include <raylib.h>
#include <stdio.h>

#include "tiled.h"

#define SCREEN_W 1280
#define SCREEN_H 720

void updateCamera(Vector2 *offset, float *zoom) {
    if (IsKeyDown(KEY_UP)) offset->y += 16.0f / *zoom;
    if (IsKeyDown(KEY_DOWN)) offset->y -=  16.0f/ *zoom;
    if (IsKeyDown(KEY_RIGHT)) offset->x -= 16.0f / *zoom;
    if (IsKeyDown(KEY_LEFT)) offset->x += 16.0f / *zoom;

    if (IsKeyDown(KEY_W)) *zoom += *zoom * 0.01f;
    if (IsKeyDown(KEY_S)) *zoom -= *zoom * 0.01f;
}

void drawOverlay() {
    DrawText(TextFormat("FPS: %d", GetFPS()), 12, 12, 24, DARKGRAY);
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

void initTiled(Tiled *tiled, TiledMap tiledMap) {
    tiled->offset = (Vector2) {0, 0};
    tiled->zoom = 64;
    tiled->mapSize[0] = tiled->tilemapTexture.width;
    tiled->mapSize[1] = tiled->tilemapTexture.height;
    tiled->targetTexture = LoadRenderTexture(1, 1);

    tiled->atlasSize[0] = tiledMap.atlasSize[0];
    tiled->atlasSize[1] = tiledMap.atlasSize[1];

    textureFromPixels(&tiled->atlasTexture,
            tiledMap.atlasData,
            tiledMap.atlasSize[0] * tiledMap.tilesize,
            tiledMap.atlasSize[1] * tiledMap.tilesize);

    renderTilemapTexture(&tiled->tilemapTexture, tiledMap);

    initTiledShader(tiled);
}

void setTiledShaderUniforms(Tiled tiled) {
    SetShaderValue(tiled.shader, tiled.offsetLoc, &tiled.offset, SHADER_UNIFORM_VEC2);
    SetShaderValue(tiled.shader, tiled.zoomLoc, &tiled.zoom, SHADER_UNIFORM_FLOAT);

    SetShaderValue(tiled.shader, tiled.atlasSizeLoc, &tiled.atlasSize, SHADER_UNIFORM_IVEC2);
    SetShaderValue(tiled.shader, tiled.mapSizeLoc, &tiled.tilemapTexture.width, SHADER_UNIFORM_IVEC2);

    SetShaderValueTexture(tiled.shader, tiled.atlasTextureLoc, tiled.atlasTexture);
    SetShaderValueTexture(tiled.shader, tiled.tilemapTextureLoc, tiled.tilemapTexture);
}

void unloadTiled(Tiled *tiled) {
    UnloadShader(tiled->shader);
    UnloadTexture(tiled->atlasTexture);
    UnloadTexture(tiled->tilemapTexture);
    UnloadRenderTexture(tiled->targetTexture);
}

void drawTiled(Tiled *tiled) {
    updateCamera(&tiled->offset, &tiled->zoom);

    BeginShaderMode(tiled->shader);
    setTiledShaderUniforms(*tiled);

    DrawTextureRec(tiled->targetTexture.texture,
            (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
            (Vector2){0, 0},
            WHITE);
    EndShaderMode();
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_W, SCREEN_H, "tiled");

    Tiled tiled;
    TiledMap tiledMap = loadTiledMap("map.tiles");
    initTiled(&tiled, tiledMap);



    while (!WindowShouldClose()) {

        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        drawTiled(&tiled);
        drawOverlay();

        EndDrawing();
    }

    unloadTiled(&tiled);

    CloseWindow();
    return 0;
}
