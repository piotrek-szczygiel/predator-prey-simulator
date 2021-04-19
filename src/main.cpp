#include <raylib.h>
#include "ResourceManager.h"
#include "simulation/Simulation.h"

int main() {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(1024, 720, "sim");
    SetTargetFPS(60);
    Camera2D camera{};
    camera.zoom = 1.5f;

    ResourceManager::the().load_all_textures();
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

    ResourceManager::the().free_all_textures();
    CloseWindow();

    return 0;
}