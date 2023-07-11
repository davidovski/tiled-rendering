#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "tiled.h"

int lastSelectedTile[2] = {0, 0};
int selectedTile[2] = {0, 0};

int mode = -1;

void drawOverlay(Tiled tiled) {
    Vector2 screenPos = translateTiledScreenPosition(tiled, (Vector2){selectedTile[0], selectedTile[1]});
    
    DrawRectangleLinesEx((Rectangle) {
                screenPos.x,
                screenPos.y,
                tiled.zoom,
                tiled.zoom
            }, tiled.zoom/16, GREEN);
}

void modifyTile(Tiled *tiled, int tile) {
    setTiledMapTile(tiled->tiledMap, selectedTile, tile);
    redrawTiledMap(*tiled);
}

void setDrawMode(Tiled *tiled, int tile) {
    mode = tile % tiled->tiledMap.tileCount;
    if (mode < 0) mode += tiled->tiledMap.tileCount;
    printf("mode: %d\n", tiled->tiledMap.tileCount);
    modifyTile(tiled, mode);
}

void handleInputs(Tiled *tiled) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        setDrawMode(tiled, getTiledMapTile(tiled->tiledMap, selectedTile) + 1);

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        setDrawMode(tiled, getTiledMapTile(tiled->tiledMap, selectedTile) - 1);

    if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
        setDrawMode(tiled, 0);

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) ||
        IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) ||
        IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE)) {
        mode = -1;
    }


    if (!(selectedTile[0] == lastSelectedTile[0] &&
          selectedTile[1] == lastSelectedTile[1])) {
        if (mode != -1) modifyTile(tiled, mode);
    }
}

void update(Tiled *tiled) {
    updateTiledCamera(tiled);
    Vector2 mousePos = GetMousePosition();
    Vector2 mapPos = translateTiledPosition(*tiled, mousePos);
    if (mapPos.x >= 0 && mapPos.x < tiled->tiledMap.width
        && mapPos.y >= 0 && mapPos.y < tiled->tiledMap.height) {
        lastSelectedTile[0] = selectedTile[0];
        lastSelectedTile[1] = selectedTile[1];
        selectedTile[0] = mapPos.x;
        selectedTile[1] = mapPos.y;
    }
    handleInputs(tiled);
}

TiledMap launchEditor(TiledMap tiledMap) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_W, SCREEN_H, "tiled");

    Tiled tiled = initTiled(tiledMap);

    while (!WindowShouldClose()) {
        update(&tiled);

        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        drawTiled(&tiled);
        drawOverlay(tiled);
        DrawFPS(16, 16);

        EndDrawing();
    }

    unloadTiled(&tiled);

    CloseWindow();
    return tiled.tiledMap;
}

int main() {
    TiledMap tiledMap = loadTiledMap("map.tiles");
    printf("the top left is %d\n", tiledMap.tilelayout[0]);
    TiledMap editedTiledMap = launchEditor(tiledMap);
    printf("the top left is %d\n", editedTiledMap.tilelayout[0]);
    saveTiledMap("map.tiles", tiledMap);
}
