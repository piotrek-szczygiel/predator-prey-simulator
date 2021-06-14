#include "platform.h"
#include <raylib.h>
#include <raymath.h>
#include "version.h"

void Platform::start() {
    SetTraceLogLevel(LOG_INFO);
    SetTraceLogCallback([](int logLevel, const char* text, va_list args) {
        const char* level = "";
        switch (logLevel) {
            case LOG_TRACE: level = "[RAYLIB TRACE]\t"; break;
            case LOG_DEBUG: level = "[RAYLIB DEBUG]\t"; break;
            case LOG_INFO: level = "[RAYLIB INFO]\t"; break;
            case LOG_WARNING: level = "[RAYLIB WARNING]\t"; break;
            case LOG_ERROR: level = "[RAYLIB ERROR]\t"; break;
            case LOG_FATAL: level = "[RAYLIB FATAL]\t"; break;
            default: break;
        }
        fprintf(stderr, "%s", level);
        vfprintf(stderr, text, args);
        fprintf(stderr, "\n");
    });

    InitWindow(m_config.window_width, m_config.window_height, "Predator-Prey simulator v" PROJECT_VERSION);
    SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
    SetExitKey(KEY_Q);
    if (m_config.window_maximized) MaximizeWindow();

    m_camera.target = {(float)m_config.sim_width * m_config.tile_size / 2.0f,
                       (float)m_config.sim_height * m_config.tile_size / 2.0f};
    m_camera.zoom = m_config.window_zoom;

    m_tex_grass = LoadTexture("assets/grass.png");
    m_tex_chicken = LoadTexture("assets/chicken.png");
    m_tex_wolf = LoadTexture("assets/wolf.png");

    m_icon = LoadImage("assets/icon32.png");
    SetWindowIcon(m_icon);

    m_gui.load_style(m_config.window_style);
    reload();
}

void Platform::stop() {
    UnloadTexture(m_tex_grass);
    UnloadTexture(m_tex_chicken);
    UnloadTexture(m_tex_wolf);
    UnloadImage(m_icon);
    CloseWindow();
}

void Platform::reload() {
    if (IsWindowReady()) {
        if (!IsWindowMaximized()) {
            SetWindowSize(m_config.window_width, m_config.window_height);
        }

        SetTargetFPS(m_config.window_fps);
        m_gui.plot_clear();
    }
}

bool Platform::should_close() {
    return WindowShouldClose();
}

bool Platform::should_restart() const {
    return m_gui.should_restart() || IsKeyPressed(KEY_R);
}

bool Platform::should_tick() {
    return IsKeyPressed(KEY_SPACE);
}

TimePoint Platform::time_now() {
    return std::chrono::steady_clock::now();
}

double Platform::time_diff_ms(TimePoint t1, TimePoint t2) {
    std::chrono::duration<double, std::milli> diff = t1 - t2;
    return diff.count();
}

void Platform::interact(const Simulation& sim) {
    if (m_gui.closed()) {
        m_camera.offset.x = (float)GetScreenWidth() / 2.0f;
    } else {
        m_camera.offset.x = m_gui.bounds().x / 2.0f;
    }

    if (m_config.control_plot) {
        m_camera.offset.y = m_gui.bounds_plot().y / 2.0f;
    } else {
        m_camera.offset.y = ((float)GetScreenHeight() - m_gui.status_height()) / 2.0f;
    }

    m_hl_pos = {-1, -1};
    m_config.window_width = GetScreenWidth();
    m_config.window_height = GetScreenHeight();
    m_config.window_maximized = IsWindowMaximized();
    m_config.window_zoom = m_camera.zoom;

    if (IsKeyPressed(KEY_F11)) IsWindowMaximized() ? RestoreWindow() : MaximizeWindow();
    if (IsKeyPressed(KEY_ESCAPE)) m_gui.toggle();
    if (IsKeyPressed(KEY_P)) m_config.control_pause ^= 1;
    if (IsKeyPressed(KEY_H)) m_config.window_help ^= 1;

    Vector2 mouse = GetMousePosition();

    if (!m_gui.on_gui(mouse)) {
        m_camera.zoom = std::clamp(m_camera.zoom + GetMouseWheelMove() * 0.1f, 0.1f, 2.0f);

        Vector2 delta = Vector2Subtract(m_prev_mouse_pos, mouse);
        m_prev_mouse_pos = mouse;
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            m_camera.target = GetScreenToWorld2D(Vector2Add(m_camera.offset, delta), m_camera);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            m_camera.target = {(float)m_config.sim_width * m_config.tile_size / 2.0f,
                               (float)m_config.sim_height * m_config.tile_size / 2.0f};
        }

        Vector2 ms = GetScreenToWorld2D(mouse, m_camera);
        Vec2 mw = {(int)(ms.x / m_config.tile_size), (int)(ms.y / m_config.tile_size)};
        if (!sim.out_of_map(mw)) {
            m_hl_pos = mw;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                m_hl_agent = sim.at(m_hl_pos);
            }
        }

        if (m_hl_agent && m_hl_agent->is_dead()) m_hl_agent = nullptr;
        m_gui.set_observed_agent(m_hl_agent);
    }
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
        case AgentType::Chicken: return YELLOW;
        case AgentType::Wolf: return RED;
        default: return BLACK;
    }
}

void Platform::start_drawing(Simulation& sim) {
    BeginDrawing();
    ClearBackground({230, 230, 230, 255});
    BeginMode2D(m_camera);

    const Color grass_color = {99, 171, 63, 255};
    const Rectangle grass_rectangle = {0, 0, (float)sim.size().x * m_config.tile_size,
                                       (float)sim.size().y * m_config.tile_size};
    DrawRectangleRec(grass_rectangle, grass_color);
    DrawRectangleLinesEx({-4.0f, -4.0f, (float)sim.size().x * m_config.tile_size + 8.0f,
                          (float)sim.size().y * m_config.tile_size + 8.0f},
                         4.0f, BEIGE);

    if (m_hl_agent) {
        DrawRectangleLinesEx({(float)m_hl_agent->pos.x * m_config.tile_size,
                              (float)m_hl_agent->pos.y * m_config.tile_size, m_config.tile_size, m_config.tile_size},
                             1.0f, RED);
    }

    if (m_hl_pos != Vec2{-1, -1}) {
        DrawRectangleRec({(float)m_hl_pos.x * m_config.tile_size, (float)m_hl_pos.y * m_config.tile_size,
                          m_config.tile_size, m_config.tile_size},
                         Fade(YELLOW, 0.5f));
    }

    for (const auto& chunk : sim.chunks()) {
        for (const auto& agent : chunk) {
            if (m_camera.zoom >= 0.75f) {
                if (m_config.control_debug && agent.type != AgentType::Grass && !agent.hungry) {
                    DrawRectangleRec(
                        {(float)agent.pos.x * m_config.tile_size + 1, (float)agent.pos.y * m_config.tile_size + 1,
                         m_config.tile_size - 2, m_config.tile_size - 2},
                        Color{173, 216, 230, 128});
                }
                DrawTextureRec(texture_for_type(agent.type), {0, 0, m_config.tile_size, m_config.tile_size},
                               {(float)agent.pos.x * m_config.tile_size, (float)agent.pos.y * m_config.tile_size},
                               WHITE);
            } else {
                DrawRectangleRec(
                    {(float)agent.pos.x * m_config.tile_size + 1, (float)agent.pos.y * m_config.tile_size + 1,
                     m_config.tile_size - 2, m_config.tile_size - 2},
                    color_for_type(agent.type));
            }
        }
    }
}

void Platform::update_gui_end_drawing(const Simulation& sim) {
    EndMode2D();
    if (!m_gui.draw_observed_agent()) m_hl_agent = nullptr;
    m_gui.update(sim);
    EndDrawing();
}

void draw_rect(Agent* a, Color color) {
    if (a && !a->is_dead())
        DrawRectangleV({(float)a->pos.x * 16.0f, (float)a->pos.y * 16.0f}, {16, 16}, Fade(color, 0.3f));
}

void Platform::draw_debug(Simulation& sim) const {
    const int w = sim.chunk_size().x * (int)m_config.tile_size;
    const int h = sim.chunk_size().y * (int)m_config.tile_size;
    for (int y = 0; y < sim.chunk_count().y; ++y) {
        for (int x = 0; x < sim.chunk_count().x; ++x) {
            DrawRectangleLines(x * w, y * h, w, h, Fade(WHITE, 0.25f));
        }
    }

    for (const auto& line : sim.debug_lines()) {
        if (line.from->is_dead() || line.to->is_dead()) continue;
        auto color = BLACK;
        if (line.from->type == AgentType::Chicken && line.to->type == AgentType::Grass) {
            color = GREEN;
        } else if (line.from->type == AgentType::Chicken && line.to->type == AgentType::Wolf) {
            color = DARKGRAY;
        } else if (line.from->type == AgentType::Wolf && line.to->type == AgentType::Chicken) {
            color = RED;
        } else if (line.from->type == line.to->type) {
            color = PINK;
        }
        Vector2 from = {(float)line.from->pos.x * 16 + 8, (float)line.from->pos.y * 16 + 8};
        Vector2 to = {(float)line.to->pos.x * 16 + 8, (float)line.to->pos.y * 16 + 8};
        DrawLineEx(from, to, 2.0f, Fade(color, 0.5f));
    }

    for (const auto& breed : sim.debug_breeds()) {
        draw_rect(breed.mom, VIOLET);
        draw_rect(breed.dad, MAGENTA);
        draw_rect(breed.kid, YELLOW);
    }
}
