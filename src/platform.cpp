#include "platform.h"

void Platform::start() {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(m_config.window_width, m_config.window_height, "Predator-Prey simulation");
    SetTargetFPS(m_config.window_fps);

    m_camera.offset = {(float)m_config.window_width / 2.0f, (float)m_config.window_height / 2.0f};
    m_camera.rotation = 0;
    m_camera.target = {(float)m_config.sim_width * m_config.tile_size / 2.0f,
                       (float)m_config.sim_height * m_config.tile_size / 2.0f};
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

void Platform::reload() {
    if (IsWindowReady()) {
        SetWindowSize(m_config.window_width, m_config.window_height);
        SetTargetFPS(m_config.window_fps);
    }
}

bool Platform::should_close() {
    return WindowShouldClose();
}

bool Platform::should_restart() {
    return IsKeyPressed(KEY_R);
}

void Platform::interact() {}

float Platform::time() {
    return (float)GetTime();
}

Texture2D Platform::texture_for_type(AgentType type) {
    switch (type) {
        case AgentType::Cabbage: return m_tex_cabbage;
        case AgentType::Chicken: return m_tex_chicken;
        case AgentType::Wolf: return m_tex_wolf;
        default: return m_tex_ground;
    }
}

void Platform::start_drawing(Simulation& sim) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode2D(m_camera);

    for (int y = 0; y < m_config.sim_height; ++y) {
        for (int x = 0; x < m_config.sim_width; ++x) {
            float wx = (float)x * m_config.tile_size;
            float wy = (float)y * m_config.tile_size;
            DrawTextureRec(m_tex_ground, {0, 0, m_config.tile_size, m_config.tile_size}, {wx, wy}, WHITE);

            auto agent = sim.at(x, y);
            if (agent) {
                auto tint = WHITE;
                if (agent->energy)
                    DrawTextureRec(texture_for_type(agent->type), {0, 0, m_config.tile_size, m_config.tile_size},
                                   {wx, wy}, tint);
            }
        }
    }
}

void Platform::end_drawing() {
    EndMode2D();
    EndDrawing();
}