#pragma once
#include <raylib.h>
#include "simulation.h"

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
    float time();
    void start_drawing(Simulation& sim);
    void update_gui_end_drawing();

    template <typename... Args>
    void title(const char* fmt, Args... args) {
        SetWindowTitle(TextFormat(fmt, args...));
    }

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
    bool m_gui_closed = true;
    bool m_gui_restart = false;
    std::string m_status{};

    void update_gui();
};