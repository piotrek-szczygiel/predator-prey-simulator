#include "platform.h"
#include <raylib.h>
#include <raymath.h>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "external/raygui.h"

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
    m_tex_wolf = LoadTexture("assets/wolf.png");

    GuiLoadStyle("assets/candy.rgs");
    GuiFade(0.8f);
}

void Platform::stop() {
    UnloadTexture(m_tex_grass);
    UnloadTexture(m_tex_chicken);
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
    return m_gui_restart || IsKeyPressed(KEY_R);
}

bool Platform::should_tick() {
    return IsKeyPressed(KEY_SPACE);
}

void Platform::interact() {
    m_camera.offset = {(float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f};
    m_gui_width = GetScreenWidth() / 3;

    if (IsKeyPressed(KEY_F11)) IsWindowMaximized() ? RestoreWindow() : MaximizeWindow();
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) m_gui_closed = !m_gui_closed;

    float mouse_delta = GetMouseWheelMove();
    float new_zoom = m_camera.zoom + mouse_delta * 0.1f;
    if (new_zoom <= 0.1f) new_zoom = 0.1f;
    if (new_zoom >= 2.0f) new_zoom = 2.0f;
    m_camera.zoom = new_zoom;

    Vector2 current_mouse_pos = GetMousePosition();
    Vector2 delta = Vector2Subtract(m_prev_mouse_pos, current_mouse_pos);
    m_prev_mouse_pos = current_mouse_pos;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        (m_gui_closed || current_mouse_pos.x < (int)GetScreenWidth() - m_gui_width)) {
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
        default: return {};
    }
}

Color Platform::color_for_type(AgentType type) {
    switch (type) {
        case AgentType::Grass: return LIME;
        case AgentType::Chicken: return ORANGE;
        case AgentType::Wolf: return GRAY;
        default: return BLACK;
    }
}

void Platform::start_drawing(Simulation& sim) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode2D(m_camera);

    const Color GRASS_COLOR = {99, 171, 63, 255};
    const Rectangle GRASS_REC = {0, 0, (float)m_config.sim_width * m_config.tile_size,
                                 (float)m_config.sim_height * m_config.tile_size};
    DrawRectangleRec(GRASS_REC, GRASS_COLOR);

    for (const auto& chunk : sim.chunks()) {
        for (const auto& agent : chunk) {
            if (m_camera.zoom >= 0.5f) {
                DrawTextureRec(texture_for_type(agent.type), {0, 0, m_config.tile_size, m_config.tile_size},
                               {(float)agent.x * m_config.tile_size, (float)agent.y * m_config.tile_size}, WHITE);
            } else {
                DrawRectangleRec({(float)agent.x * m_config.tile_size, (float)agent.y * m_config.tile_size,
                                  m_config.tile_size, m_config.tile_size},
                                 color_for_type(agent.type));
            }
        }
    }
}

void Platform::update_gui_end_drawing() {
    EndMode2D();
    if (!m_gui_closed) update_gui();
    EndDrawing();
}

void Platform::update_gui() {
    auto width = (float)m_gui_width;
    float margin = 5.0f;
    float padding = 10.0f;
    float x = (float)GetScreenWidth() - width;
    float y = margin;
    GuiStatusBar({0, (float)GetScreenHeight() - 20, (float)GetScreenWidth(), 20}, m_status.c_str());
    m_gui_closed = GuiWindowBox({x, y, width - margin, (float)GetScreenHeight() - 20 - margin * 2.0f}, "Settings");

    x += padding;
    y += 30;
    width -= padding * 2.0f;

    if (GuiButton({x, y, width / 2.0f - padding / 2.0f, 30}, GuiIconText(RICON_FILE_SAVE, "Save config"))) {
        if (m_config.write()) {
            m_status = "Configuration saved successfully!";
        } else {
            m_status = "Error while saving configuration!";
        }
    }
    if (GuiButton({x + width / 2.0f + padding / 2.0f, y, width / 2.0f - padding, 30},
                  GuiIconText(RICON_FILE_OPEN, "Load config"))) {
        if (m_config.load()) {
            m_status = "Configuration loaded successfully!";
        } else {
            m_status = "Error while loading configuration!";
        }
    }
    y += 30 + padding * 4.0f;

    GuiGroupBox({x, y, width, (float)GetScreenHeight() - y - 20 - margin * 2.0f - padding}, "Simulation");
    x += padding;
    width -= padding * 2.0f;
    y += padding * 2.0f;

    m_gui_restart = GuiButton({x + width / 4.0f, y, width / 2.0f, 30}, GuiIconText(RICON_REDO, "Restart"));
    y += 30 + padding;

    GuiLine({x, y, width, 1}, nullptr);
    y += padding;

    float x_slider = x + 50;
    m_config.window_tick_time_ms =
        (int)GuiSlider({x_slider, y, width / 2.0f, 20.0f}, "Tick", TextFormat("%dms", m_config.window_tick_time_ms),
                       (float)m_config.window_tick_time_ms, 0, 1000);
}