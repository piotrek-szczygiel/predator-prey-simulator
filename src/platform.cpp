#include "platform.h"
#include <raymath.h>

void Platform::start() {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(m_config.window_width, m_config.window_height, "Predator-Prey simulation");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    if (m_config.window_maximized) MaximizeWindow();
    SetTargetFPS(m_config.window_fps);

    m_camera.target = {(float)m_config.sim_width * m_config.tile_size / 2.0f,
                       (float)m_config.sim_height * m_config.tile_size / 2.0f};
    m_camera.zoom = 1.0;

    m_tex_grass = LoadTexture("assets/grass.png");
    m_tex_chicken = LoadTexture("assets/chicken.png");
    m_tex_ground = LoadTexture("assets/ground.png");
    m_tex_wolf = LoadTexture("assets/wolf.png");
}

void Platform::stop() {
    UnloadTexture(m_tex_grass);
    UnloadTexture(m_tex_chicken);
    UnloadTexture(m_tex_ground);
    UnloadTexture(m_tex_wolf);
    CloseWindow();
}

void Platform::reload() {
    if (IsWindowReady()) {
        if (!IsWindowMaximized()) {
            SetWindowSize(m_config.window_width, m_config.window_height);
        }

        SetTargetFPS(m_config.window_fps);
    }
}

bool Platform::should_close() {
    return WindowShouldClose();
}

bool Platform::should_restart() {
    return IsKeyPressed(KEY_R);
}

bool Platform::should_tick() {
    return IsKeyPressed(KEY_SPACE);
}

void Platform::interact() {
    m_camera.offset = {(float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f};

    if (IsKeyPressed(KEY_F11)) IsWindowMaximized() ? RestoreWindow() : MaximizeWindow();

    float mouse_delta = GetMouseWheelMove();
    float new_zoom = m_camera.zoom + mouse_delta * 0.1f;
    if (new_zoom <= 0.25f) new_zoom = 0.25f;
    if (new_zoom >= 4.0f) new_zoom = 4.0f;
    m_camera.zoom = new_zoom;

    Vector2 current_mouse_pos = GetMousePosition();
    Vector2 delta = Vector2Subtract(m_prev_mouse_pos, current_mouse_pos);
    m_prev_mouse_pos = current_mouse_pos;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        m_camera.target = GetScreenToWorld2D(Vector2Add(m_camera.offset, delta), m_camera);
    }

    if (IsWindowResized() || IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        m_camera.target = {(float)m_config.sim_width * m_config.tile_size / 2.0f,
                           (float)m_config.sim_height * m_config.tile_size / 2.0f};
        m_camera.zoom = 1.0f;
    }
}

float Platform::time() {
    return (float)GetTime();
}

Texture2D Platform::texture_for_type(AgentType type) {
    switch (type) {
        case AgentType::Grass: return m_tex_grass;
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