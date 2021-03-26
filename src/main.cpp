#include <raylib.h>
#include "map/Map.h"

int main() {
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(800, 600, "sim");

    SetTargetFPS(60);

    Map map;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        map.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}