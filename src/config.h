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
    int window_fps{};

    int runtime_tick_time_ms{};
    bool runtime_manual_stepping{};
    bool runtime_debug_draw{};

    bool seed_manual{};
    char seed[SEED_SIZE]{};

    int genes_max_offsprings{};
    int genes_max_sensor_range{};

    int sim_width{};
    int sim_height{};
    int sim_chunk_width{};
    int sim_chunk_height{};

    int sim_energy_start{};
    int sim_energy_tick_loss{};
    int sim_energy_breed_needed{};
    int sim_energy_breed_loss{};

    int grass_spawn_time{};
    int grass_spawn_count{};
    int grass_nutrition_value{};

    int chicken_spawn_count{};
    int chicken_sensor_range{};
    int chicken_nutrition_value{};
    int chicken_hunger_start{};
    int chicken_hunger_stop{};

    int wolf_spawn_count{};
    int wolf_sensor_range{};
    int wolf_hunger_start{};
    int wolf_hunger_stop{};

    bool valid = false;

   private:
    const char* m_filename;

    mINI::INIFile m_file;
    mINI::INIStructure m_ini;

    enum class LinkType { Int, Bool };

    struct Link {
        LinkType type;
        union {
            int* out_int;
            bool* out_bool;
        };
        const char* section;
        const char* name;
    };
    std::vector<Link> m_links;

    void set(int& output, const char* section, const char* name);
    void set(bool& output, const char* section, const char* name);
};