#include <raylib.h>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include "ResourceManager.h"
#include "Util.h"
#include "simulation/Simulation.h"

int main(int argc, char **argv) {
    const bool video = !(argc == 2 && strcmp(argv[1], "--no-video") == 0);

    constexpr int max_depth{5};
    bool assets_found = false;
    for (int i = 0; i < max_depth; ++i) {
        if (std::filesystem::exists("assets")) {
            assets_found = true;
            break;
        }

        std::filesystem::current_path("..");
    }

    if (!assets_found) std::cerr << "Unable to find assets directory";

    Camera2D camera{};
    if (video) {
        if (!assets_found) return 1;

        SetTraceLogLevel(LOG_WARNING);
        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "sim");
        SetTargetFPS(60);

        camera.zoom = 1.5f;
        camera.offset = {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
        camera.target = MIDDLE;

        ResourceManager::the().load_all_textures();
    }

    auto simulation = std::make_unique<Simulation>();

    std::filesystem::remove_all("csv");
    std::filesystem::create_directory("csv");
    auto csv = get_csv_stream();

    int restart_count = 0;
    constexpr int max_restarts{10};

    auto simulation_start = std::chrono::system_clock::now();
    constexpr std::chrono::duration<double> max_simulation_time{120.0};

    for (;;) {
        simulation->update();

        auto predator_count = simulation->get_agent_count(AgentType::WOLF);
        auto prey_count = simulation->get_agent_count(AgentType::CHICKEN);
        csv << predator_count << ";" << prey_count << "\n";

        if ((video && IsKeyPressed(KEY_R)) ||
            (!video && ((std::chrono::system_clock::now() - simulation_start > max_simulation_time) || predator_count < 2))) {
            simulation = std::make_unique<Simulation>();
            csv = get_csv_stream();
            simulation_start = std::chrono::system_clock::now();
            ++restart_count;
        }

        if (video) {
            if (WindowShouldClose()) break;

            update_camera(camera);
            BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(camera);
            simulation->draw();
            EndMode2D();
            EndDrawing();
        }

        if (!video && restart_count >= max_restarts - 1) break;
    }

    if (video) {
        ResourceManager::the().free_all_textures();
        CloseWindow();
    }

    return 0;
}