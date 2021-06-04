#include "platform.h"
#include <raymath.h>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include <raygui.h>

void Platform::start() {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(m_config.window_width, m_config.window_height, "Predator-Prey simulation");
    SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
    SetExitKey(KEY_Q);
    if (m_config.window_maximized) MaximizeWindow();

    m_camera.target = {(float)m_config.sim_width * m_config.tile_size / 2.0f,
                       (float)m_config.sim_height * m_config.tile_size / 2.0f};
    m_camera.zoom = 1.0;

    m_tex_grass = LoadTexture("assets/grass.png");
    m_tex_chicken = LoadTexture("assets/chicken.png");
    m_tex_wolf = LoadTexture("assets/wolf.png");

    reload();
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
        if (m_config.window_style == 0) {
            GuiLoadStyleDefault();
        } else {
            GuiLoadStyle(m_styles[m_config.window_style]);
        }
        GuiFade(0.95f);
    }
}

bool Platform::should_close() {
    return WindowShouldClose();
}

bool Platform::should_restart() const {
    return m_gui_restart || IsKeyPressed(KEY_R);
}

bool Platform::should_tick() {
    return IsKeyPressed(KEY_SPACE);
}

TimePoint Platform::time_now() {
    return std::chrono::high_resolution_clock::now();
}

double Platform::time_diff_ms(TimePoint t1, TimePoint t2) {
    std::chrono::duration<double, std::milli> diff = t1 - t2;
    return diff.count();
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
        case AgentType::Wolf: return DARKGRAY;
        default: return BLACK;
    }
}

void Platform::start_drawing(Simulation& sim) {
    BeginDrawing();
    ClearBackground({230, 230, 230, 255});
    BeginMode2D(m_camera);

    const Color GRASS_COLOR = {99, 171, 63, 255};
    const Rectangle GRASS_REC = {0, 0, (float)sim.size().x * m_config.tile_size,
                                 (float)sim.size().y * m_config.tile_size};
    DrawRectangleRec(GRASS_REC, GRASS_COLOR);
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
    update_gui(sim);
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
        Color color;
        if (line.from->type == AgentType::Chicken && line.to->type == AgentType::Grass)
            color = ORANGE;
        else if (line.from->type == AgentType::Chicken && line.to->type == AgentType::Wolf)
            color = BLACK;
        else if (line.from->type == AgentType::Wolf && line.to->type == AgentType::Chicken)
            color = RED;
        else if (line.from->type == line.to->type)
            color = VIOLET;
        else
            color = WHITE;
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

void Platform::update_gui(const Simulation& sim) {
    auto width = (float)m_gui_width;
    float margin = 5.0f;
    float padding = 10.0f;

    float status_bar_height = 20.0f;
    float button_height = 40.0f;
    float entry_height = 25.0f;
    float checkbox_size = 15.0f;

    if (m_gui_closed) {
        m_camera.offset = {(float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f};
    } else {
        m_camera.offset = {(float)(GetScreenWidth() - m_gui_width) / 2.0f, (float)(GetScreenHeight() - 20) / 2.0f};
    }
    m_gui_width = GetScreenWidth() / 3;
    if (m_gui_width < 500) m_gui_width = 500;

    m_config.window_width = GetScreenWidth();
    m_config.window_height = GetScreenHeight();
    m_config.window_maximized = IsWindowMaximized() ? 1 : 0;

    if (IsKeyPressed(KEY_F11)) IsWindowMaximized() ? RestoreWindow() : MaximizeWindow();
    if (IsKeyPressed(KEY_ESCAPE)) m_gui_closed = !m_gui_closed;

    float mouse_delta = GetMouseWheelMove();
    float new_zoom = m_camera.zoom + mouse_delta * 0.1f;
    if (new_zoom <= 0.1f) new_zoom = 0.1f;
    if (new_zoom >= 4.0f) new_zoom = 4.0f;
    m_camera.zoom = new_zoom;

    Vector2 current_mouse_pos = GetMousePosition();
    Vector2 delta = Vector2Subtract(m_prev_mouse_pos, current_mouse_pos);
    m_prev_mouse_pos = current_mouse_pos;

    m_hl_pos = {-1, -1};
    if (m_gui_closed || (int)current_mouse_pos.x < GetScreenWidth() - m_gui_width) {
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            m_camera.target = GetScreenToWorld2D(Vector2Add(m_camera.offset, delta), m_camera);
        }

        Vector2 ms = GetScreenToWorld2D(current_mouse_pos, m_camera);
        Vec2 mw = {(int)(ms.x / m_config.tile_size), (int)(ms.y / m_config.tile_size)};
        if (!sim.out_of_map(mw)) {
            m_hl_pos = mw;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                m_hl_agent = sim.at(m_hl_pos);
            }
        }
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        m_camera.target = {(float)m_config.sim_width * m_config.tile_size / 2.0f,
                           (float)m_config.sim_height * m_config.tile_size / 2.0f};
        m_camera.zoom = 1.0f;
    }

    if (m_hl_agent && m_hl_agent->is_dead()) m_hl_agent = nullptr;
    if (m_hl_agent) {
        float i_width = 150.0f;
        float i_height = 150.0f;

        if (GuiWindowBox({margin, margin, i_width, i_height}, GuiIconText(RICON_INFO, "Agent info"))) {
            m_hl_agent = nullptr;
        } else {
            float x = margin + padding;
            float y = margin + 20;

            GuiLabel({x, y, i_width, entry_height}, TextFormat("Type: %s", m_hl_agent->type_str()));
            y += entry_height;

            GuiLabel({x, y, i_width, entry_height},
                     TextFormat("Position: %d, %d", m_hl_agent->pos.x, m_hl_agent->pos.y));
            y += entry_height;

            GuiLabel({x, y, i_width, entry_height}, TextFormat("Energy: %d", m_hl_agent->energy));
            y += entry_height;

            GuiLabel({x, y, i_width, entry_height}, TextFormat("Hungry: %s", m_hl_agent->hungry ? "yes" : "no"));
            y += entry_height;

            if (m_hl_agent->type == AgentType::Wolf && m_hl_agent->random_direction != Vec2{}) {
                GuiLabel({x, y, i_width, entry_height}, TextFormat("Random dir: %d, %d", m_hl_agent->random_direction.x,
                                                                   m_hl_agent->random_direction.y));
            }
        }
    }

    if (m_gui_closed) return;

    static bool s_style_edit = false;
    if (s_style_edit) GuiLock();

    float x = (float)GetScreenWidth() - width;
    float y = margin;
    static std::string s_status{};
    GuiStatusBar({0, (float)GetScreenHeight() - status_bar_height, (float)GetScreenWidth(), status_bar_height},
                 s_status.c_str());

    m_gui_closed =
        GuiWindowBox({x, y, width - margin, (float)GetScreenHeight() - status_bar_height - margin * 2.0f}, "Settings");

    x += padding;
    y += 20 + padding * 2.0f;
    width -= padding * 2.0f;

    if (GuiButton({x + 2.0f * padding, y, width / 2.0f - 4.0f * padding, button_height},
                  GuiIconText(RICON_FILE_SAVE, "Save config"))) {
        if (m_config.write()) {
            s_status = "Configuration saved successfully!";
        } else {
            s_status = "Error while saving configuration!";
        }
    }
    if (GuiButton({x + width / 2.0f + 2.0f * padding, y, width / 2.0f - 4.0f * padding, button_height},
                  GuiIconText(RICON_FILE_OPEN, "Reload config"))) {
        if (m_config.load()) {
            s_status = "Configuration loaded successfully!";
            reload();
        } else {
            s_status = "Error while loading configuration!";
        }
    }
    y += button_height + padding * 2.0f;

    GuiLine({x - padding, y, width + padding * 2.0f - margin, 1}, "Runtime");
    y += padding * 2.0f;

    m_gui_restart = GuiButton({x + 2.0f * padding, y, width / 2.0f - 4.0f * padding, button_height},
                              GuiIconText(RICON_REDO, "Restart"));

    m_config.runtime_debug_draw = GuiCheckBox({x + width / 2.0f + 2.0f * padding, y, checkbox_size, checkbox_size},
                                              "Debug draw", m_config.runtime_debug_draw);
    y += entry_height;

    m_config.runtime_manual_stepping = GuiCheckBox({x + width / 2.0f + 2.0f * padding, y, checkbox_size, checkbox_size},
                                                   "Manual stepping", m_config.runtime_manual_stepping);
    y += entry_height;

    static bool s_seed_edit = false;
    if (!m_config.seed_manual) {
        GuiSetState(GUI_STATE_DISABLED);
        GuiTextBox({x + 3.0f * padding + entry_height, y, 100, entry_height}, (char*)TextFormat("%u", sim.seed()), 0,
                   false);
        GuiSetState(GUI_STATE_NORMAL);
        if (GuiButton({x + 2.0f * padding, y, entry_height, entry_height}, GuiIconText(RICON_FILE_COPY, nullptr))) {
            std::snprintf(m_config.seed, Config::SEED_SIZE, "%u", sim.seed());
            SetClipboardText(m_config.seed);
        }
    } else if (GuiTextBox({x + 2.0f * padding, y, 100 + padding + entry_height, entry_height}, m_config.seed,
                          Config::SEED_SIZE, s_seed_edit)) {
        s_seed_edit = !s_seed_edit;
    }

    m_config.seed_manual =
        GuiCheckBox({x + 4.0f * padding + entry_height + 100, y + entry_height / 4.0f, checkbox_size, checkbox_size},
                    "Seed", m_config.seed_manual);

    static bool s_tick_edit = false;
    if (!m_config.runtime_manual_stepping) {
        if (GuiSpinner({x + width * 3.0f / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Tick (ms)",
                       &m_config.runtime_tick_time_ms, 0, 1000, s_tick_edit)) {
            s_tick_edit = !s_tick_edit;
        }
    }

    y += button_height + padding;

    GuiLine({x - padding, y, (width + padding * 2.0f - margin) / 2.0f, 1}, "Statistics");
    GuiLine({x - padding + (width + padding * 2.0f - margin) / 2.0f, y, (width + padding * 2.0f - margin) / 2.0f, 1},
            "Graphics");
    y += padding * 2.0f;

    GuiLabel({x + width / 2.0f + padding, y + 1.0f, width / 2.0f, entry_height}, TextFormat("FPS: %d", GetFPS()));
    static bool s_fps_edit = false;
    if (GuiSpinner({x + 5.0f * width / 6.0f - 2.0f * padding, y, width / 6.0f, entry_height}, "Max",
                   &m_config.window_fps, 0, 999, s_fps_edit)) {
        s_fps_edit = !s_fps_edit;
        reload();
    }

    Rectangle styles_position = {x + width * 3.0f / 4.0f - 2.0f * padding, y + entry_height + padding, width / 4.0f,
                                 entry_height};
    GuiLabel({x + width / 2.0f + padding, y + entry_height + padding, width / 2.0f, entry_height}, "GUI Style");

    GuiLabel({x + padding, y, width, entry_height}, TextFormat("Current tick: %u", sim.ticks()));
    y += entry_height;

    GuiLabel({x + padding, y, width, entry_height}, TextFormat("Update time: %.3fms", sim.avg_update_time()));
    y += entry_height;

    GuiLabel({x + padding, y, width, entry_height}, TextFormat("Wolf count: %d", sim.count(AgentType::Wolf)));
    y += entry_height;

    GuiLabel({x + padding, y, width, entry_height}, TextFormat("Chicken count: %d", sim.count(AgentType::Chicken)));
    y += entry_height;

    GuiLabel({x + padding, y, width, entry_height}, TextFormat("Grass count: %d", sim.count(AgentType::Grass)));
    y += entry_height + padding * 4.0f;

    GuiLine({x - padding, y, width + padding * 2.0f - margin, 1}, "Simulation");
    y += padding * 2.0f;

    static bool s_grid_width_edit = false;
    if (GuiSpinner({x + width / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Grid width",
                   &m_config.sim_width, 10, 500, s_grid_width_edit)) {
        s_grid_width_edit = !s_grid_width_edit;
    }

    static bool s_grid_height_edit = false;
    if (GuiSpinner({x + width * 3.0f / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Grid height",
                   &m_config.sim_height, 10, 500, s_grid_height_edit)) {
        s_grid_height_edit = !s_grid_height_edit;
    }
    y += entry_height + padding;

    static bool s_chunk_width_edit = false;
    if (GuiSpinner({x + width / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Chunk width",
                   &m_config.sim_chunk_width, 4, 100, s_chunk_width_edit)) {
        s_chunk_width_edit = !s_chunk_width_edit;
    }

    static bool s_chunk_height_edit = false;
    if (GuiSpinner({x + width * 3.0f / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Chunk height",
                   &m_config.sim_chunk_height, 4, 100, s_chunk_height_edit)) {
        s_chunk_height_edit = !s_chunk_height_edit;
    }
    y += entry_height + padding;

    static bool s_energy_start_edit = false;
    if (GuiSpinner({x + width / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Energy start",
                   &m_config.sim_energy_start, 1, 1000, s_energy_start_edit)) {
        s_energy_start_edit = !s_energy_start_edit;
    }

    static bool s_energy_loss_edit = false;
    if (GuiSpinner({x + width * 3.0f / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Energy loss",
                   &m_config.sim_energy_tick_loss, 1, 1000, s_energy_loss_edit)) {
        s_energy_loss_edit = !s_energy_loss_edit;
    }
    y += entry_height + padding;

    static bool s_energy_breed_needed_edit = false;
    if (GuiSpinner({x + width / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Breed energy",
                   &m_config.sim_energy_breed_needed, 1, 1000, s_energy_breed_needed_edit)) {
        s_energy_breed_needed_edit = !s_energy_breed_needed_edit;
    }

    static bool s_energy_breed_loss_edit = false;
    if (GuiSpinner({x + width * 3.0f / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Breed loss",
                   &m_config.sim_energy_breed_loss, 1, 1000, s_energy_breed_loss_edit)) {
        s_energy_breed_loss_edit = !s_energy_breed_loss_edit;
    }
    y += entry_height + padding * 3.0f;

    GuiLine({x - padding, y, width + padding * 2.0f - margin, 1}, "Grass");
    y += padding * 2.0f;

    static bool s_grass_spawn_time_edit = false;
    if (GuiSpinner({x + width / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Frequency",
                   &m_config.grass_spawn_time, 1, 1000, s_grass_spawn_time_edit)) {
        s_grass_spawn_time_edit = !s_grass_spawn_time_edit;
    }

    static bool s_grass_spawn_count_edit = false;
    if (GuiSpinner({x + width * 3.0f / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Spawn count",
                   &m_config.grass_spawn_count, 1, 1000, s_grass_spawn_count_edit)) {
        s_grass_spawn_count_edit = !s_grass_spawn_count_edit;
    }
    y += entry_height + padding;

    static bool s_grass_nutrition_value_edit = false;
    if (GuiSpinner({x + width / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Nutrition",
                   &m_config.grass_nutrition_value, 1, 1000, s_grass_nutrition_value_edit)) {
        s_grass_nutrition_value_edit = !s_grass_nutrition_value_edit;
    }
    y += entry_height + padding * 3.0f;

    GuiLine({x - padding, y, width + padding * 2.0f - margin, 1}, "Chicken");
    y += padding * 2.0f;

    static bool s_chicken_spawn_count_edit = false;
    if (GuiSpinner({x + width / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Spawn count",
                   &m_config.chicken_spawn_count, 1, 1000, s_chicken_spawn_count_edit)) {
        s_chicken_spawn_count_edit = !s_chicken_spawn_count_edit;
    }

    static bool s_chicken_sensor_range_edit = false;
    if (GuiSpinner({x + width * 3.0f / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Sensor range",
                   &m_config.chicken_sensor_range, 1, 1000, s_chicken_sensor_range_edit)) {
        s_chicken_sensor_range_edit = !s_chicken_sensor_range_edit;
    }
    y += entry_height + padding;

    static bool s_chicken_hunger_start_edit = false;
    if (GuiSpinner({x + width / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Hunger start",
                   &m_config.chicken_hunger_start, 1, 1000, s_chicken_hunger_start_edit)) {
        s_chicken_hunger_start_edit = !s_chicken_hunger_start_edit;
    }

    static bool s_chicken_hunger_stop_edit = false;
    if (GuiSpinner({x + width * 3.0f / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Hunger stop",
                   &m_config.chicken_hunger_stop, 1, 1000, s_chicken_hunger_stop_edit)) {
        s_chicken_hunger_stop_edit = !s_chicken_hunger_stop_edit;
    }
    y += entry_height + padding;

    static bool s_chicken_nutrition_value_edit = false;
    if (GuiSpinner({x + width / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Nutrition",
                   &m_config.chicken_nutrition_value, 1, 1000, s_chicken_nutrition_value_edit)) {
        s_chicken_nutrition_value_edit = !s_chicken_nutrition_value_edit;
    }
    y += entry_height + padding * 3.0f;

    GuiLine({x - padding, y, width + padding * 2.0f - margin, 1}, "Wolf");
    y += padding * 2.0f;

    static bool s_wolf_spawn_count_edit = false;
    if (GuiSpinner({x + width / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Spawn count",
                   &m_config.wolf_spawn_count, 0, 1000, s_wolf_spawn_count_edit)) {
        s_wolf_spawn_count_edit = !s_wolf_spawn_count_edit;
    }

    static bool s_wolf_sensor_range_edit = false;
    if (GuiSpinner({x + width * 3.0f / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Sensor range",
                   &m_config.wolf_sensor_range, 1, 1000, s_wolf_sensor_range_edit)) {
        s_wolf_sensor_range_edit = !s_wolf_sensor_range_edit;
    }
    y += entry_height + padding;

    static bool s_wolf_hunger_start_edit = false;
    if (GuiSpinner({x + width / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Hunger start",
                   &m_config.wolf_hunger_start, 1, 1000, s_wolf_hunger_start_edit)) {
        s_wolf_hunger_start_edit = !s_wolf_hunger_start_edit;
    }

    static bool s_wolf_hunger_stop_edit = false;
    if (GuiSpinner({x + width * 3.0f / 4.0f - 2.0f * padding, y, width / 4.0f, entry_height}, "Hunger stop",
                   &m_config.wolf_hunger_stop, 1, 1000, s_wolf_hunger_stop_edit)) {
        s_wolf_hunger_stop_edit = !s_wolf_hunger_stop_edit;
    }

    GuiUnlock();
    if (GuiDropdownBox(styles_position, m_style_names, &m_config.window_style, s_style_edit)) {
        s_style_edit = !s_style_edit;
        reload();
    }
}