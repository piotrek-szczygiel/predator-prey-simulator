#include <raylib.h>
#include "ResourceManager.h"
#include "Util.h"
#include "simulation/Simulation.h"

int main() {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "sim");
    SetTargetFPS(60);

    Camera2D camera{};
    camera.zoom = 1.5f;
    camera.offset = {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
    camera.target = MIDDLE;

    ResourceManager::the().load_all_textures();
    Simulation simulation;

    while (!WindowShouldClose()) {
        update_camera(camera);
        simulation.update();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);
        simulation.draw();
        EndMode2D();
        EndDrawing();
    }

    ResourceManager::the().free_all_textures();
    CloseWindow();

    return 0;
}