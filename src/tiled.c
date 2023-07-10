#include <raylib.h>
#include <stdio.h>

#include "tiledfile.h"
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

Shader initTiledShader(TiledUniforms *uniforms) {
    Shader shader = LoadShader(0, "tiled.glsl");

    uniforms->offsetLoc = GetShaderLocation(shader, "offset");
    uniforms->zoomLoc = GetShaderLocation(shader, "zoom");

    uniforms->atlasSizeLoc = GetShaderLocation(shader, "atlasSize");
    uniforms->mapSizeLoc = GetShaderLocation(shader, "mapSize");

    uniforms->atlasTextureLoc = GetShaderLocation(shader, "atlasTexture");
    uniforms->tilemapTextureLoc = GetShaderLocation(shader, "tilemapTexture");

    return shader;
}

void setTiledShaderUniforms(Shader shader, TiledUniforms uniforms) {
    SetShaderValue(shader, uniforms.offsetLoc, &uniforms.offset, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, uniforms.zoomLoc, &uniforms.zoom, SHADER_UNIFORM_FLOAT);

    SetShaderValue(shader, uniforms.atlasSizeLoc, &uniforms.atlasSize, SHADER_UNIFORM_IVEC2);
    SetShaderValue(shader, uniforms.mapSizeLoc, &uniforms.tilemapTexture.width, SHADER_UNIFORM_IVEC2);


    SetShaderValueTexture(shader, uniforms.atlasTextureLoc, uniforms.atlasTexture);
    SetShaderValueTexture(shader, uniforms.tilemapTextureLoc, uniforms.tilemapTexture);
}

void unloadTiledShader(Shader shader, TiledUniforms uniforms) {
    UnloadShader(shader);
    UnloadTexture(uniforms.atlasTexture);
    UnloadTexture(uniforms.tilemapTexture);

}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_W, SCREEN_H, "tiled");

    TiledUniforms uniforms;
    if (loadTileMap("map.tiles", &uniforms.tilemapTexture, &uniforms.atlasTexture, uniforms.atlasSize))
        return 1;

    uniforms.offset = (Vector2) {0, 0};
    uniforms.zoom = 64;
    uniforms.mapSize[0] = uniforms.tilemapTexture.width;
    uniforms.mapSize[1] = uniforms.tilemapTexture.height;

    RenderTexture2D target = LoadRenderTexture(1, 1);
    Shader shader = initTiledShader(&uniforms);


    while (!WindowShouldClose()) {
        updateCamera(&uniforms.offset, &uniforms.zoom);

        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        BeginShaderMode(shader);
            setTiledShaderUniforms(shader, uniforms);

            DrawTextureRec(target.texture,
                    (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
                    (Vector2){0, 0},
                    WHITE);
        EndShaderMode();

        drawOverlay();

        EndDrawing();
    }

    UnloadRenderTexture(target);
    unloadTiledShader(shader, uniforms);

    CloseWindow();
    return 0;
}
