#include "gui.h"
#include <string>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include <raygui.h>

void Gui::load_style(int window_style) {
    if (window_style == 0) {
        GuiLoadStyleDefault();
        m_status = "Loaded default style";
    } else {
        GuiLoadStyle(m_styles[window_style]);
        m_status = std::string(TextFormat("Loaded style: %s", m_styles[window_style]));
    }
    GuiSetStyle(SPINNER, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_RIGHT);
    GuiFade(0.9f);
}

bool Gui::on_gui(Vector2 pos) const {
    if (!m_closed && CheckCollisionPointRec(pos, m_bounds)) return true;
    if (m_config.control_plot && CheckCollisionPointRec(pos, m_bounds_plot)) return true;
    if (m_agent && CheckCollisionPointRec(pos, m_bounds_agent)) return true;
    if ((m_config.window_help || m_about) && CheckCollisionPointRec(pos, m_bounds_msg)) return true;
    return false;
}

void Gui::plot_add(int chicken, int wolf) {
    if (m_plot_data.size() == 10000) m_plot_data.pop_front();
    m_plot_data.emplace_back(chicken, wolf);
}

void Gui::update(const Simulation& sim) {
    auto screen_h = (float)GetScreenHeight();
    auto screen_w = (float)GetScreenWidth();

    GuiStatusBar({0, screen_h - STATUS_HEIGHT, screen_w, STATUS_HEIGHT}, m_status.c_str());

    const float GUI_W = 500.0f;
    const float GUI_H = 1000.0f;
    const float GUI_M = 15.0f;
    const float GUI_BORDER =
        20.f * (float)GuiGetStyle(DEFAULT, BORDER_WIDTH) + (float)GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);
    const float GUI_P = 10.0f;
    m_bounds = {screen_w - GUI_W - GUI_M, GUI_M, GUI_W, screen_h - 2 * GUI_M - STATUS_HEIGHT};

    const float MSG_W = 350;
    const float MSG_H = m_config.window_help ? 470.0f : 420.0f;
    m_bounds_msg = {(screen_w - (m_closed ? 0 : GUI_W + GUI_M) - MSG_W) / 2,
                    (screen_h - (m_config.control_plot ? m_bounds_plot.height + GUI_M : 0) - STATUS_HEIGHT - MSG_H) / 2,
                    MSG_W, MSG_H};
    Vector2 msg_offset = {m_bounds_msg.x + GUI_P, m_bounds_msg.y + GUI_P + GUI_BORDER};

    if (!m_closed) {
        Rectangle view = GuiScrollPanel(m_bounds, {0, 0, GUI_W - GUI_BORDER, GUI_H}, &m_scroll);
        BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);
        draw_controls(sim, view.x + m_scroll.x + GUI_P, view.y + m_scroll.y + GUI_P, view.width - 2 * GUI_P);
        EndScissorMode();
    }

    if (m_config.control_plot) {
        const float PLOT_H = screen_h / 4;
        m_bounds_plot = {GUI_M, screen_h - PLOT_H - STATUS_HEIGHT - GUI_M + 1,
                         (m_closed ? screen_w : m_bounds.x) - 2 * GUI_M, PLOT_H};

        float x = m_bounds_plot.x + 1;
        float y = m_bounds_plot.y + 27;
        float w = m_bounds_plot.width - 2;
        float h = m_bounds_plot.height - 30;

        int max = 0;
        for (const auto& [chicken, wolf] : m_plot_data) max = std::max(max, std::max(chicken, wolf));

        m_config.control_plot ^=
            GuiWindowBox(m_bounds_plot, GuiIconText(RICON_PENCIL, TextFormat("Population plot (max y=%d)", max)));

        float dx = w / (float)m_plot_data.size();
        float dy = h / (float)max;

        for (int i = 0; i < m_plot_data.size(); ++i) {
            auto [chicken, wolf] = m_plot_data[i];
            DrawPixelV({x + dx * (float)i, y + h - dy * (float)chicken}, BLUE);
            DrawPixelV({x + dx * (float)i, y + h - dy * (float)wolf}, RED);
        }
    }

    if (m_config.window_help) {
        m_config.window_help ^= GuiWindowBox(m_bounds_msg, GuiIconText(RICON_HELP, "Help"));
        GuiLabel({msg_offset.x, msg_offset.y, 0, 0}, m_help_text);
    }

    if (m_about) {
        m_about ^= GuiWindowBox(m_bounds_msg, GuiIconText(RICON_INFO, "About"));
        GuiLabel({msg_offset.x, msg_offset.y, 0, 0}, m_about_text);
    }
}

bool Gui::draw_observed_agent() {
    if (!m_agent) return false;

    const float M = 15.0f;
    const float P = 10.0f;
    const float ROW_H = 25.0f;

    m_bounds_agent = {M, M, 150, m_agent->type == AgentType::Grass ? 95.0f : 170.0f};

    if (GuiWindowBox(m_bounds_agent, GuiIconText(RICON_INFO, "Agent info"))) {
        m_agent = nullptr;
        return false;
    }

    float x = M + P;
    float y = M + 20;
    float w = m_bounds_agent.width;

    GuiLabel({x, y, w, ROW_H}, TextFormat("Type: %s", m_agent->type_str()));
    y += ROW_H;

    GuiLabel({x, y, w, ROW_H}, TextFormat("Position: %d, %d", m_agent->pos.x, m_agent->pos.y));
    y += ROW_H;

    GuiLabel({x, y, w, ROW_H}, TextFormat("Energy: %d", m_agent->energy));
    y += ROW_H;

    if (m_agent->type != AgentType::Grass) {
        GuiLabel({x, y, w, ROW_H}, TextFormat("Hungry: %s", m_agent->hungry ? "yes" : "no"));
        y += ROW_H;

        GuiLabel({x, y, w, ROW_H}, TextFormat("Offsprings: %d", m_agent->genes.offsprings));
        y += ROW_H;

        GuiLabel({x, y, w, ROW_H}, TextFormat("Sensor range: %d", m_agent->genes.sensor_range));
    }

    return true;
}

void Gui::draw_controls(const Simulation& sim, float x, float y, float w) {
    const float P = 10.0f;
    const float BEFORE_LINE = P * 4;
    const float AFTER_LINE = P * 2;
    const float ROW_H = 25.0f;
    const float BTN_W = w / 5;

    if (m_edit.style) GuiLock();

    GuiLine({x, y, w, 1}, "Settings");
    y += AFTER_LINE;

    {
        float yy = y;
        if (GuiButton({x, y, BTN_W, ROW_H}, GuiIconText(RICON_FILE_SAVE, "Save"))) {
            m_status = m_config.write() ? "Configuration saved" : "Unable to save configuration!";
        }

        if (GuiButton({x + w / 4, y, BTN_W, ROW_H}, GuiIconText(RICON_FILE_OPEN, "Load"))) {
            m_status = m_config.load() ? "Configuration loaded" : "Unable to load configuration!";
        }
        yy += ROW_H + P;

        if (GuiButton({x, yy, BTN_W, ROW_H}, GuiIconText(RICON_HELP, "Help"))) {
            m_config.window_help ^= 1;
            m_about = false;
        }

        if (GuiButton({x + w / 4, yy, BTN_W, ROW_H}, GuiIconText(RICON_INFO, "About"))) {
            m_about ^= 1;
            m_config.window_help = false;
        }
    }

    GuiLabel({x + w / 2, y, w / 4, ROW_H}, TextFormat("FPS: %d", GetFPS()));
    if (GuiSpinner({x + 3 * w / 4, y, BTN_W, ROW_H}, "Max", &m_config.window_fps, 0, 999, m_edit.fps)) {
        m_edit.fps ^= 1;
        SetTargetFPS(m_config.window_fps);
        m_status = "Max FPS set to " + std::to_string(m_config.window_fps);
    }
    y += ROW_H + P;

    GuiLabel({x + w / 2, y, w / 4, ROW_H}, "GUI Style:");
    Rectangle style_dropdown_position = {x + 3 * w / 4, y, BTN_W, ROW_H};
    y += ROW_H;

    y += BEFORE_LINE;
    GuiLine({x, y, w, 1}, "Control");
    y += AFTER_LINE;

    m_restart = GuiButton({x, y, BTN_W, ROW_H}, GuiIconText(RICON_REDO_FILL, "Restart"));
    if (GuiButton({x + w / 4, y, BTN_W, ROW_H}, GuiIconText(RICON_EXIT, "Exit"))) {
        CloseWindow();
    }

    m_config.control_pause =
        GuiCheckBox({x + w / 2, y + ROW_H / 4, ROW_H / 2, ROW_H / 2}, "Pause", m_config.control_pause);

    m_config.control_plot =
        GuiCheckBox({x + 4 * w / 6, y + ROW_H / 4, ROW_H / 2, ROW_H / 2}, "Plot", m_config.control_plot);

    m_config.control_debug =
        GuiCheckBox({x + 5 * w / 6, y + ROW_H / 4, ROW_H / 2, ROW_H / 2}, "Debug", m_config.control_debug);
    y += ROW_H + P;

    if (!m_config.seed_manual) {
        GuiSetState(GUI_STATE_DISABLED);
        GuiTextBox({x + ROW_H + P, y, 100, ROW_H}, (char*)TextFormat("%u", sim.seed()), 0, false);
        GuiSetState(GUI_STATE_NORMAL);
        if (GuiButton({x, y, ROW_H, ROW_H}, GuiIconText(RICON_FILE_COPY, nullptr))) {
            std::snprintf(m_config.seed, Config::SEED_SIZE, "%u", sim.seed());
            SetClipboardText(m_config.seed);
            m_status = "Seed copied to clipboard";
        }
    } else {
        m_edit.seed ^= GuiTextBox({x, y, 100 + ROW_H + P, ROW_H}, m_config.seed, Config::SEED_SIZE, m_edit.seed);
    }

    m_config.seed_manual =
        GuiCheckBox({x + 100 + ROW_H + 2 * P, y + ROW_H / 4, ROW_H / 2, ROW_H / 2}, "Seed", m_config.seed_manual);

    m_edit.tick ^=
        GuiSpinner({x + w / 2, y, BTN_W, ROW_H}, "Tick (ms)", &m_config.control_tick_time, 0, 5000, m_edit.tick);
    y += ROW_H;

    y += BEFORE_LINE;
    GuiLine({x, y, w / 2, 1}, "Statistics");
    GuiLine({x + w / 2, y, w / 2, 1}, "Dimensions");
    y += AFTER_LINE;

    {
        float yy = y;
        m_edit.sim_width ^=
            GuiSpinner({x + w / 2, yy, BTN_W, ROW_H}, "Grid width", &m_config.sim_width, 10, 500, m_edit.sim_width);
        yy += ROW_H + P;

        m_edit.sim_height ^=
            GuiSpinner({x + w / 2, yy, BTN_W, ROW_H}, "Grid height", &m_config.sim_height, 10, 500, m_edit.sim_height);
        yy += ROW_H + P;

        m_edit.chunk_width ^= GuiSpinner({x + w / 2, yy, BTN_W, ROW_H}, "Chunk width", &m_config.sim_chunk_width, 2,
                                         100, m_edit.chunk_width);
        yy += ROW_H + P;

        m_edit.chunk_height ^= GuiSpinner({x + w / 2, yy, BTN_W, ROW_H}, "Chunk height", &m_config.sim_chunk_height, 2,
                                          100, m_edit.chunk_height);
    }

    GuiLabel({x, y, w / 2, ROW_H}, TextFormat("Current tick: %u", sim.ticks()));
    y += ROW_H;

    GuiLabel({x, y, w / 2, ROW_H}, TextFormat("Update time: %.3fms", sim.avg_update_time()));
    y += ROW_H;

    GuiLabel({x, y, w / 2, ROW_H}, TextFormat("Chicken count: %d", sim.count(AgentType::Chicken)));
    y += ROW_H;

    GuiLabel({x, y, w / 2, ROW_H}, TextFormat("Wolf count: %d", sim.count(AgentType::Wolf)));
    y += ROW_H;

    auto [avg_offsprings, avg_sensor_range] = sim.avg_genes();
    GuiLabel({x, y, w / 2, ROW_H}, TextFormat("Avg. offsprings: %.1f", avg_offsprings));
    y += ROW_H;

    GuiLabel({x, y, w / 2, ROW_H}, TextFormat("Avg. sensor range: %.1f", avg_sensor_range));
    y += ROW_H;

    y += BEFORE_LINE;
    GuiLine({x, y, w, 1}, "Genes");
    y += AFTER_LINE;

    m_edit.genes_offsprings ^= GuiSpinner({x, y, BTN_W, ROW_H}, "Max offsprings", &m_config.genes_max_offsprings, 1, 8,
                                          m_edit.genes_offsprings);
    m_edit.genes_sensor_range ^= GuiSpinner({x + w / 2, y, BTN_W, ROW_H}, "Max sensor range",
                                            &m_config.genes_max_sensor_range, 1, 100, m_edit.genes_sensor_range);
    y += ROW_H;

    y += BEFORE_LINE;
    GuiLine({x, y, w, 1}, "Grass");
    y += AFTER_LINE;

    m_edit.grass_spawn ^=
        GuiSpinner({x, y, BTN_W, ROW_H}, "Spawn count", &m_config.grass_spawn_count, 1, 1000, m_edit.grass_spawn);
    m_edit.grass_nutrition ^= GuiSpinner({x + w / 2, y, BTN_W, ROW_H}, "Nutritional value",
                                         &m_config.grass_nutritional_value, 1, 1000, m_edit.grass_nutrition);
    y += ROW_H;

    y += BEFORE_LINE;
    GuiLine({x, y, w, 1}, "Chicken");
    y += AFTER_LINE;

    m_edit.chicken_spawn ^=
        GuiSpinner({x, y, BTN_W, ROW_H}, "Spawn count", &m_config.chicken_spawn_count, 1, 1000, m_edit.chicken_spawn);
    m_edit.chicken_sensor ^= GuiSpinner({x + w / 2, y, BTN_W, ROW_H}, "Sensor range", &m_config.chicken_sensor_range, 1,
                                        100, m_edit.chicken_sensor);
    y += ROW_H + P;

    m_edit.chicken_energy_start ^= GuiSpinner({x, y, BTN_W, ROW_H}, "Energy start", &m_config.chicken_energy_start, 100,
                                              10000, m_edit.chicken_energy_start);
    m_edit.chicken_energy_loss ^= GuiSpinner({x + w / 2, y, BTN_W, ROW_H}, "Energy loss", &m_config.chicken_energy_loss,
                                             1, 100, m_edit.chicken_energy_loss);
    y += ROW_H + P;

    m_edit.chicken_hunger_start ^= GuiSpinner({x, y, BTN_W, ROW_H}, "Hunger start", &m_config.chicken_hunger_start, 1,
                                              10000, m_edit.chicken_hunger_start);
    m_edit.chicken_hunger_stop ^= GuiSpinner({x + w / 2, y, BTN_W, ROW_H}, "Hunger stop", &m_config.chicken_hunger_stop,
                                             1, 10000, m_edit.chicken_hunger_stop);
    y += ROW_H + P;

    m_edit.chicken_breed ^=
        GuiSpinner({x, y, BTN_W, ROW_H}, "Breed cost", &m_config.chicken_breed_cost, 1, 10000, m_edit.chicken_breed);
    m_edit.chicken_nutrition ^= GuiSpinner({x + w / 2, y, BTN_W, ROW_H}, "Nutritional value",
                                           &m_config.chicken_nutritional_value, 1, 10000, m_edit.chicken_nutrition);
    y += ROW_H;

    y += BEFORE_LINE;
    GuiLine({x, y, w, 1}, "Wolf");
    y += AFTER_LINE;

    m_edit.wolf_spawn ^=
        GuiSpinner({x, y, BTN_W, ROW_H}, "Spawn count", &m_config.wolf_spawn_count, 1, 1000, m_edit.wolf_spawn);
    m_edit.wolf_sensor ^= GuiSpinner({x + w / 2, y, BTN_W, ROW_H}, "Sensor range", &m_config.wolf_sensor_range, 1, 100,
                                     m_edit.wolf_sensor);
    y += ROW_H + P;

    m_edit.wolf_energy_start ^= GuiSpinner({x, y, BTN_W, ROW_H}, "Energy start", &m_config.wolf_energy_start, 100,
                                           10000, m_edit.wolf_energy_start);
    m_edit.wolf_energy_loss ^= GuiSpinner({x + w / 2, y, BTN_W, ROW_H}, "Energy loss", &m_config.wolf_energy_loss, 1,
                                          100, m_edit.wolf_energy_loss);
    y += ROW_H + P;

    m_edit.wolf_hunger_start ^= GuiSpinner({x, y, BTN_W, ROW_H}, "Hunger start", &m_config.wolf_hunger_start, 1, 10000,
                                           m_edit.wolf_hunger_start);
    m_edit.wolf_hunger_stop ^= GuiSpinner({x + w / 2, y, BTN_W, ROW_H}, "Hunger stop", &m_config.wolf_hunger_stop, 1,
                                          10000, m_edit.wolf_hunger_stop);
    y += ROW_H + P;

    m_edit.wolf_breed ^=
        GuiSpinner({x, y, BTN_W, ROW_H}, "Breed cost", &m_config.wolf_breed_cost, 1, 10000, m_edit.wolf_breed);

    GuiUnlock();
    if (GuiDropdownBox(style_dropdown_position, m_style_names, &m_config.window_style, m_edit.style)) {
        m_edit.style ^= 1;
        load_style(m_config.window_style);
    }
}