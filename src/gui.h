#pragma once
#include <raylib.h>
#include <deque>
#include "simulation.h"

class Gui {
   public:
    explicit Gui(Config& config) : m_config(config) {}

    Rectangle bounds() const { return m_bounds; }
    Rectangle bounds_plot() const { return m_bounds_plot; }
    float status_height() const { return STATUS_HEIGHT; }
    bool closed() const { return m_closed; }
    bool draw_observed_agent();
    void load_style(int window_style);
    void toggle() { m_closed ^= 1; }
    void update(const Simulation& sim);
    bool should_restart() const { return m_restart; }
    void set_observed_agent(const Agent* agent) { m_agent = agent; }
    bool on_gui(Vector2 pos) const;
    void plot_add(int chicken, int wolf);
    void plot_clear() { m_plot_data.clear(); }

   private:
    Config& m_config;

    bool m_closed = false;

    Rectangle m_bounds{};
    Rectangle m_bounds_msg{};
    Rectangle m_bounds_plot{};
    Vector2 m_scroll{};

    const float STATUS_HEIGHT = 20.0f;

    std::string m_status{};
    bool m_about = false;
    bool m_restart = false;

    std::deque<std::pair<int, int>> m_plot_data{};

    const Agent* m_agent{};

    struct {
        bool fps;
        bool style;
        bool seed;
        bool tick;
        bool sim_width;
        bool sim_height;
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

    const char* m_help_text =
        "Welcome to Predator-Prey simulator!\n\n"
        "Keybindings:\n"
        "  [ESC] - toggle settings window\n"
        "  [H] - toggle help\n"
        "  [Q] - quit program\n"
        "  [F11] - toggle maximize window\n"
        "  [R] - restart simulation\n"
        "  [P] - control_pause simulation\n"
        "  [SPACE] - next tick when paused\n"
        "  [F12] - screenshot\n"
        "  [CTRL]+[F12] - record gif\n\n"
        "Mouse controls:\n"
        "  Left click - select agent to observe\n"
        "  Right click - panning\n"
        "  Middle click - center camera\n"
        "  Scroll wheel - zoom in and out";

    const char* m_about_text =
        "Predator-Prey simulator is a project made for\n"
        "System Modeling and Simulation class\n"
        "at AGH University by:\n\n"
        "\t- Piotr Szczygiel\n"
        "\t- Marcin Wojsik\n\n"
        "Third party libraries used:\n"
        "\t- raylib  (github.com/raysan5/raylib)\n"
        "\t- raygui  (github.com/raysan5/raygui)\n"
        "\t- mINI    (github.com/pulzed/mINI)\n"
        "\t- CRCpp   (github.com/d-bahr/CRCpp)\n"
        "\t- itlib   (github.com/iboB/itlib)";

    void draw_controls(const Simulation& sim, float x, float y, float w);
};