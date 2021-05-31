#pragma once
#include <raylib.h>
#include "simulation.h"

class Platform {
   public:
    Platform(const Config& config) : m_config(config) {}

    void start();
    void stop();
    static bool running();
    void start_drawing(Simulation& sim);
    void end_drawing();

   private:
    const Config& m_config;

    Camera2D m_camera{};

    Texture2D m_tex_cabbage{};
    Texture2D m_tex_chicken{};
    Texture2D m_tex_ground{};
    Texture2D m_tex_wolf{};

    Texture2D texture_for_type(AgentType type);
};