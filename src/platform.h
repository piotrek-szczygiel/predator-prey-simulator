#pragma once
#include <raylib.h>
#include <chrono>
#include "simulation.h"

using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>>;

class Platform {
   public:
    explicit Platform(Config& config) : m_config(config) {}
    void start();
    void stop();
    void reload();
    bool should_close();
    bool should_restart();
    bool should_tick();
    void interact();
    TimePoint time_now();
    double time_diff_ms(TimePoint t1, TimePoint t2);
    void start_drawing(Simulation& sim);
    void draw_debug(Simulation& sim);
    void update_gui_end_drawing(const Simulation& sim);

   private:
    Config& m_config;

    Camera2D m_camera{};

    Texture2D m_tex_grass{};
    Texture2D m_tex_chicken{};
    Texture2D m_tex_wolf{};

    Vector2 m_prev_mouse_pos{};

    Texture2D texture_for_type(AgentType type);
    Color color_for_type(AgentType type);

    int m_gui_width{};
    bool m_gui_closed = false;
    bool m_gui_restart = false;

    void update_gui(const Simulation& sim);
};