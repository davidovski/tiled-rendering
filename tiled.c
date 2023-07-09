#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <wctype.h>

#define SCREEN_W 1280
#define SCREEN_H 720

#define MAP_W 16
#define MAP_H 4

const int tilemap[MAP_H * MAP_W] = {
   0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0,
   0, 3, 3, 0, 0, 3, 1, 1, 1, 3, 0, 0, 0, 0, 0, 0,
   3, 1, 1, 3, 3, 1, 2, 2, 2, 1, 3, 3, 3, 0, 0, 3,
   1, 2, 2, 1, 1, 2, 4, 4, 4, 2, 1, 1, 1, 3, 3, 1
};                          

const int atlasSize[2] = {2, 2};

Texture2D processTilemapTexture(const int * tilemap, int width, int height) {
    Color *pixels = (Color*) malloc(width * height * sizeof(Color));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int tile = tilemap[y*width + x];
            pixels[y*width + x] = (Color){
                (int) tile, 0, 0, 0
            };
        }
    }

    Image checkedIm = {
        .data = pixels,
        .width = width,
        .height = height,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    };

    Texture2D checked = LoadTextureFromImage(checkedIm);
    UnloadImage(checkedIm);

    return checked;
}

int main() {
    InitWindow(SCREEN_W, SCREEN_H, "tiled");

    Shader shader = LoadShader(0, "tiled.glsl");

    Texture2D processedTilemap = processTilemapTexture(tilemap, MAP_W, MAP_H);
    Texture atlas = LoadTexture("tiled.png");
    RenderTexture2D target = LoadRenderTexture(SCREEN_W, SCREEN_H);

    float resolution[2] = {SCREEN_W, SCREEN_H};
    float offset[2] = {0, 0};
    float zoom = 16.0f;

    int resolutionLoc = GetShaderLocation(shader, "resolution");
    int locationLoc = GetShaderLocation(shader, "offset");
    int zoomLoc = GetShaderLocation(shader, "zoom");
    int atlasSizeLoc = GetShaderLocation(shader, "atlasSize");

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

        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        BeginTextureMode(target);
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, BLACK);
        EndTextureMode();

        BeginShaderMode(shader);
            SetShaderValueTexture(shader, textureLoc, atlas);
            SetShaderValueTexture(shader, tilemapLoc, processedTilemap);

            // draw the base image to texture0
            DrawTexture(target.texture, 0, 0, WHITE);
        EndShaderMode();
            DrawTexture(processedTilemap, 0, 0, WHITE);

            DrawText(TextFormat("FPS: %d", GetFPS()), 12, 12, 24, DARKGRAY);

        EndDrawing();
    }

    UnloadShader(shader);
    UnloadRenderTexture(target);
    UnloadTexture(atlas);

    CloseWindow();

    return 0;
}
