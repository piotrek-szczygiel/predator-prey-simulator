#pragma once
#include <raylib.h>
#include "simulation.h"

const int TILE_SIZE = 16;

class Platform {
   public:
    Platform(int win_width, int win_height, const char* title, int fps = 60)
        : m_width(win_width), m_height(win_height), m_title(title), m_fps(fps) {}

    void start(int sim_width, int sim_height);
    void stop();
    static bool running();
    void draw(const Simulation& sim);

   private:
    int m_width;
    int m_height;
    int m_fps;
    const char* m_title;

    Camera2D m_camera{};

    Texture2D m_tex_cabbage{};
    Texture2D m_tex_chicken{};
    Texture2D m_tex_ground{};
    Texture2D m_tex_wolf{};

    Texture2D texture_for_type(AgentType type);
};