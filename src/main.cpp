#include <raylib.h>
#include <memory>
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
    auto simulation = std::make_unique<Simulation>();

    while (!WindowShouldClose()) {
        update_camera(camera);
        simulation->update();

        if (IsKeyPressed(KEY_R)) {
            simulation = std::make_unique<Simulation>();
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);
        simulation->draw();
        EndMode2D();
        EndDrawing();
    }

    ResourceManager::the().free_all_textures();
    CloseWindow();

    return 0;
}