#include "platform.h"

void Platform::start(int sim_width, int sim_height) {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(m_width, m_height, m_title);
    SetTargetFPS(m_fps);

    m_camera.offset = {(float)m_width / 2.0f, (float)m_height / 2.0f};
    m_camera.rotation = 0;
    m_camera.target = {(float)sim_width * TILE_SIZE / 2.0f, (float)sim_height * TILE_SIZE / 2.0f};
    m_camera.zoom = 1.0;

    m_tex_cabbage = LoadTexture("assets/cabbage.png");
    m_tex_chicken = LoadTexture("assets/chicken.png");
    m_tex_ground = LoadTexture("assets/ground.png");
    m_tex_wolf = LoadTexture("assets/wolf.png");
}

void Platform::stop() {
    UnloadTexture(m_tex_cabbage);
    UnloadTexture(m_tex_chicken);
    UnloadTexture(m_tex_ground);
    UnloadTexture(m_tex_wolf);
    CloseWindow();
}

bool Platform::running() {
    return !WindowShouldClose();
}

Texture2D Platform::texture_for_type(AgentType type) {
    switch (type) {
        case AgentType::Cabbage: return m_tex_cabbage;
        case AgentType::Chicken: return m_tex_chicken;
        case AgentType::Wolf: return m_tex_wolf;
        default: return m_tex_ground;
    }
}

void Platform::draw(const Simulation& sim) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode2D(m_camera);

    for (int y = 0; y < sim.height(); ++y) {
        for (int x = 0; x < sim.width(); ++x) {
            float wx = x * TILE_SIZE;
            float wy = y * TILE_SIZE;
            DrawTextureRec(m_tex_ground, {0, 0, TILE_SIZE, TILE_SIZE}, {wx, wy}, WHITE);

            auto type = sim.type_at(x, y);
            if (type != AgentType::None)
                DrawTextureRec(texture_for_type(type), {0, 0, TILE_SIZE, TILE_SIZE}, {wx, wy}, WHITE);
        }
    }

    EndMode2D();
    EndDrawing();
}