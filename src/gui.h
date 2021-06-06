#pragma once
#include <raygui.h>
#include "simulation.h"

class Gui {
   public:
    explicit Gui(Config& config) : m_config(config) {}

    Rectangle bounds() { return m_bounds; }
    bool closed() { return m_closed; }
    bool draw_observed_agent();
    void load_style(int window_style);
    void toggle() { m_closed = !m_closed; }
    void update(const Simulation& sim);
    bool should_restart() const { return m_restart; }
    void set_observed_agent(const Agent* agent) { m_agent = agent; }

   private:
    Config& m_config;

    Rectangle m_bounds{};

    bool m_closed = false;
    std::string m_status{};

    Vector2 m_scroll{};

    const Agent* m_agent{};

    bool m_restart = false;

    struct {
        bool fps;
        bool style;
        bool seed;
        bool tick;
        bool grid_width;
        bool grid_height;
        bool chunk_width;
        bool chunk_height;
        bool genes_offsprings;
        bool genes_sensor_range;
        bool grass_spawn;
        bool grass_nutrition;
        bool chicken_spawn;
        bool chicken_sensor;
        bool chicken_energy_start;
        bool chicken_energy_loss;
        bool chicken_hunger_start;
        bool chicken_hunger_stop;
        bool chicken_breed;
        bool chicken_nutrition;
        bool wolf_spawn;
        bool wolf_sensor;
        bool wolf_energy_start;
        bool wolf_energy_loss;
        bool wolf_hunger_start;
        bool wolf_hunger_stop;
        bool wolf_breed;
    } m_edit{};

    const char* m_styles[9] = {
        "",
        "assets/styles/ashes.rgs",
        "assets/styles/bluish.rgs",
        "assets/styles/candy.rgs",
        "assets/styles/cherry.rgs",
        "assets/styles/cyber.rgs",
        "assets/styles/jungle.rgs",
        "assets/styles/lavanda.rgs",
        "assets/styles/terminal.rgs",
    };

    const char* m_style_names = "Default;Ashes;Bluish;Candy;Cherry;Cyber;Jungle;Lavanda;Terminal";

    void draw_controls(const Simulation& sim, float x, float y, float w);
};