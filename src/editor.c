#include <raylib.h>
#include <stdio.h>

#include "tiled.h"

int selectedTile[2] = {0, 0};

void update(Tiled *tiled) {
    updateTiledCamera(tiled);
    Vector2 mousePos = GetMousePosition();
    Vector2 mapPos = translateTiledPosition(*tiled, mousePos);
    selectedTile[0] = mapPos.x;
    selectedTile[1] = mapPos.y;
}

void drawOverlay(Tiled tiled) {
    Vector2 screenPos = translateTiledScreenPosition(tiled, (Vector2){selectedTile[0], selectedTile[1]});
    
    DrawRectangleLinesEx((Rectangle) {
                screenPos.x,
                screenPos.y,
                tiled.zoom,
                tiled.zoom
            }, tiled.zoom/16, GREEN);
}

void modifyTile(Tiled *tiled, int i) {
    if (selectedTile[0] >= 0 && selectedTile[0] < tiled->tiledMap.width
        && selectedTile[1] >= 0 && selectedTile[1] < tiled->tiledMap.height) {
        printf("%d,%d", selectedTile[0], selectedTile[1]);
        int tile = getTiledMapTile(tiled->tiledMap, selectedTile);
        int tileCount = tiled->tiledMap.atlasSize[0] * tiled->tiledMap.atlasSize[1] + 1;
        tile = (tile + i) % (tileCount);
        setTiledMapTile(tiled->tiledMap, selectedTile, tile);
        renderTilemapTexture(&tiled->tilemapTexture, tiled->tiledMap);
    }
}

int launchEditor(TiledMap tiledMap) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_W, SCREEN_H, "tiled");

    Tiled tiled = initTiled(tiledMap);

    while (!WindowShouldClose()) {
        update(&tiled);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            modifyTile(&tiled, 1);

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            modifyTile(&tiled, -1);

        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        drawTiled(&tiled);
        drawOverlay(tiled);
        DrawFPS(16, 16);

        EndDrawing();
    }

    unloadTiled(&tiled);

    CloseWindow();
    return 0;
}

int main() {
    TiledMap tiledMap = loadTiledMap("map.tiles");
    launchEditor(tiledMap);
}
