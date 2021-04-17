#include <raylib.h>
#include "simulation/Simulation.h"

int main() {
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(1024, 720, "sim");
    SetTargetFPS(60);
    Camera2D camera {};
    camera.zoom = 1.5f;

    Simulation simulation;

    while (!WindowShouldClose()) {
        simulation.update();
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        simulation.draw();
        EndMode2D();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}