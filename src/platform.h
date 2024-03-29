#pragma once
#include <raylib.h>
#include <chrono>
#include "gui.h"
#include "simulation.h"

using TimePoint = std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double>>;

class Platform {
   public:
    explicit Platform(Config& config) : m_config(config), m_gui(config) {}
    void start();
    void stop();
    void reload();
    void interact(const Simulation& sim);
    void start_drawing(Simulation& sim);
    void draw_debug(Simulation& sim) const;
    void update_gui_end_drawing(const Simulation& sim);
    Gui& gui() { return m_gui; }

    bool should_close() const;
    bool should_restart() const;
    bool should_tick() const;
    bool should_reload_script() const;
    TimePoint time_now() const;
    double time_diff_ms(TimePoint t1, TimePoint t2) const;

   private:
    Config& m_config;

    Camera2D m_camera{};

    Texture2D m_tex_grass{};
    Texture2D m_tex_chicken{};
    Texture2D m_tex_wolf{};
    Image m_icon{};

    Vector2 m_prev_mouse_pos{};

    Vec2 m_hl_pos{};
    const Agent* m_hl_agent{};

    Gui m_gui;

    Texture2D texture_for_type(AgentType type);
    Color color_for_type(AgentType type);
};
