#include <raylib.h>
#include <stdio.h>

#include "tiledfile.h"

#define SCREEN_W 1280
#define SCREEN_H 720

const int atlasSize[2] = {2, 2};

int main() {
    InitWindow(SCREEN_W, SCREEN_H, "tiled");

    Shader shader = LoadShader(0, "tiled.glsl");

    Texture2D tilemap = loadTileMap("map.tiles");

    Texture atlas = LoadTexture("atlas.png");
    RenderTexture2D target = LoadRenderTexture(SCREEN_W, SCREEN_H);

    float resolution[2] = {SCREEN_W, SCREEN_H};
    float offset[2] = {0, 0};
    float zoom = 16.0f;
    int mapSize[2] = {tilemap.width, tilemap.height};


    int resolutionLoc = GetShaderLocation(shader, "resolution");
    int locationLoc = GetShaderLocation(shader, "offset");
    int zoomLoc = GetShaderLocation(shader, "zoom");

    int atlasSizeLoc = GetShaderLocation(shader, "atlasSize");
    int mapSizeLoc = GetShaderLocation(shader, "mapSize");

    int textureLoc = GetShaderLocation(shader, "texture1");
    int tilemapLoc = GetShaderLocation(shader, "texture2");

    while (!WindowShouldClose()) {
		if (IsKeyDown(KEY_UP)) offset[1] += zoom * 0.01f;
		if (IsKeyDown(KEY_DOWN)) offset[1] -= zoom * 0.01f;
		if (IsKeyDown(KEY_RIGHT)) offset[0] -= zoom * 0.01f;
		if (IsKeyDown(KEY_LEFT)) offset[0] += zoom * 0.01f;

		if (IsKeyDown(KEY_W)) zoom -= zoom * 0.01f;
		if (IsKeyDown(KEY_S)) zoom += zoom * 0.01f;

        SetShaderValue(shader, resolutionLoc, resolution, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, locationLoc, &offset, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, zoomLoc, &zoom, SHADER_UNIFORM_FLOAT);

        SetShaderValue(shader, atlasSizeLoc, &atlasSize, SHADER_UNIFORM_IVEC2);
        SetShaderValue(shader, mapSizeLoc, &tilemap.width, SHADER_UNIFORM_IVEC2);

        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        BeginTextureMode(target);
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, BLACK);
        EndTextureMode();

        BeginShaderMode(shader);
            SetShaderValueTexture(shader, textureLoc, atlas);
            SetShaderValueTexture(shader, tilemapLoc, tilemap);

            // draw the base image to texture0
            DrawTexture(target.texture, 0, 0, WHITE);
        EndShaderMode();

            DrawText(TextFormat("FPS: %d", GetFPS()), 12, 12, 24, DARKGRAY);

        EndDrawing();
    }

    UnloadShader(shader);
    UnloadRenderTexture(target);
    UnloadTexture(atlas);

    CloseWindow();

    return 0;
}
