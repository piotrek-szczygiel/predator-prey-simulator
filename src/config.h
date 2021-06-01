#pragma once
#include <iostream>
#include "external/ini.h"

class Config {
   public:
    explicit Config(const char* filename) : m_filename(filename), m_file(filename) {}

    bool load();

    bool write() {
        for (const auto& link : m_links) m_ini[link.section][link.name] = std::to_string(link.output);
        return m_file.write(m_ini, true);
    }

    float tile_size{};

    int window_width{};
    int window_height{};
    int window_maximized{};
    int window_fps{};
    int window_tick_time_ms{};
    int window_manual_stepping{};

    int sim_width{};
    int sim_height{};
    int sim_chunk_width{};
    int sim_chunk_height{};

    int sim_energy_start{};
    int sim_energy_breed_needed{};
    int sim_energy_breed_loss{};
    int sim_energy_tick_loss{};

    int grass_spawn_time{};
    int grass_spawn_count{};
    int grass_nutrition_value{};

    int chicken_spawn_count{};
    int chicken_sensor_range{};
    int chicken_nutrition_value{};
    int chicken_hungry_start{};
    int chicken_hungry_stop{};

    int wolf_spawn_count{};
    int wolf_sensor_range{};
    int wolf_hungry_start{};
    int wolf_hungry_stop{};

    bool valid = false;

   private:
    const char* m_filename;

    mINI::INIFile m_file;
    mINI::INIStructure m_ini;

    struct Link {
        int& output;
        const char* section;
        const char* name;
    };
    std::vector<Link> m_links;

    void set(int& output, const char* section, const char* name);
};
