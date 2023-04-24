#include <raylib.h>
#include <wctype.h>

#define SCREEN_W 1280
#define SCREEN_H 720

#define MAP_W 16
#define MAP_H 4

const int tilemap[MAP_H][MAP_W] = {
    {0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0,},
    {0, 3, 3, 0, 0, 3, 1, 1, 1, 3, 0, 0, 0, 0, 0, 0,},
    {3, 1, 1, 3, 3, 1, 2, 2, 2, 1, 3, 3, 3, 0, 0, 3,},
    {1, 2, 2, 1, 1, 2, 4, 4, 4, 2, 1, 1, 1, 3, 3, 1,}
};                          

const int atlasSize[2] = {2, 2};

int main() {
    InitWindow(SCREEN_W, SCREEN_H, "tiled");

    Shader shader = LoadShader(0, "tiled.glsl");

    RenderTexture2D target = LoadRenderTexture(SCREEN_W, SCREEN_H);

    float resolution[2] = {SCREEN_W, SCREEN_H};
    float offset[2] = {0, 0};
    float zoom = 1.0f;
    Texture atlas = LoadTexture("tiles.png");
    int resolutionLoc = GetShaderLocation(shader, "resolution");
    int locationLoc = GetShaderLocation(shader, "offset");
    int zoomLoc = GetShaderLocation(shader, "zoom");
    int atlasSizeLoc = GetShaderLocation(shader, "atlasSize");

    int tilemapLoc[MAP_H][MAP_W] = {};
    for (int x = 0; x < MAP_W; x++) {
      for (int y = 0; y < MAP_H; y++) {
        tilemapLoc[y][x] =
            GetShaderLocation(shader, TextFormat("tilemap[%d][%d]", y, x));
      }
    }

    int textureLoc = GetShaderLocation(shader, "texture1");

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
        for (int x = 0; x < MAP_W; x++) {
        for (int y = 0; y < MAP_H; y++) {
            SetShaderValue(shader, tilemapLoc[y][x], &tilemap[y][x], SHADER_UNIFORM_INT);
        }
        }

        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        BeginTextureMode(target);
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, BLACK);
        EndTextureMode();

        BeginShaderMode(shader);
            SetShaderValueTexture(shader, textureLoc, atlas);

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
