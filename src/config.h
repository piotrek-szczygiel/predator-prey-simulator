#pragma once
#include <ini.h>
#include <iostream>

class Config {
   public:
    explicit Config(const char* filename) : m_filename(filename), m_file(filename) {}

    bool write();
    bool load();

    static const int SEED_SIZE = 16;

    float tile_size{};

    int window_width{};
    int window_height{};
    int window_style{};
    bool window_maximized{};
    float window_zoom{};
    int window_fps{};
    bool window_help{};

    int control_script{};
    int control_tick_time{};
    bool control_pause{};
    bool control_plot{};
    bool control_debug{};

    bool seed_manual{};
    char seed[SEED_SIZE]{};

    int genes_max_offsprings{};
    int genes_max_sensor_range{};

    int sim_width{};
    int sim_height{};
    int sim_chunk_width{};
    int sim_chunk_height{};

    int grass_spawn_count{};
    int grass_nutritional_value{};

    int chicken_spawn_count{};
    int chicken_sensor_range{};
    int chicken_energy_start{};
    int chicken_energy_loss{};
    int chicken_hunger_start{};
    int chicken_hunger_stop{};
    int chicken_breed_cost{};
    int chicken_nutritional_value{};

    int wolf_spawn_count{};
    int wolf_sensor_range{};
    int wolf_energy_start{};
    int wolf_energy_loss{};
    int wolf_hunger_start{};
    int wolf_hunger_stop{};
    int wolf_breed_cost{};

    bool valid = false;

   private:
    const char* m_filename;

    mINI::INIFile m_file;
    mINI::INIStructure m_ini;

    enum class LinkType { Int, Float, Bool };

    struct Link {
        LinkType type;
        union {
            int* out_int;
            float* out_float;
            bool* out_bool;
        };
        const char* section;
        const char* name;
    };
    std::vector<Link> m_links;

    std::string get(const char* section, const char* name);

    void set(int& output, const char* section, const char* name);
    void set(float& output, const char* section, const char* name);
    void set(bool& output, const char* section, const char* name);
};
