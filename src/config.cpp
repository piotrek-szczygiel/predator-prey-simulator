#include "config.h"

void Config::set(int& output, const char* section, const char* name) {
    std::string str_value = m_ini.get(section).get(name);
    if (str_value.empty()) {
        fprintf(stderr, "unable to find property '[%s] %s' in %s\n", section, name, m_filename);
        valid = false;
        return;
    }

    const char* value = str_value.c_str();
    char* end;
    long n = std::strtol(value, &end, 0);

    if (end <= value) {
        fprintf(stderr, "invalid value '%s' for property '[%s] %s' in %s\n", str_value.c_str(), section, name,
                m_filename);
        valid = false;
        return;
    }

    output = n;
    m_links.push_back({output, section, name});
}

bool Config::load() {
    if (!m_file.read(m_ini)) return false;

    valid = true;

    tile_size = 16.0f;

    set(window_width, "window", "width");
    set(window_height, "window", "height");
    set(window_maximized, "window", "maximized");
    set(window_fps, "window", "fps");
    set(runtime_tick_time_ms, "runtime", "tick_time_ms");
    set(runtime_manual_stepping, "runtime", "manual_stepping");
    set(runtime_debug_draw, "runtime", "debug_draw");

    set(sim_width, "simulation", "width");
    set(sim_height, "simulation", "height");
    set(sim_chunk_width, "simulation", "chunk_width");
    set(sim_chunk_height, "simulation", "chunk_height");

    set(sim_energy_start, "simulation", "energy_start");
    set(sim_energy_tick_loss, "simulation", "energy_tick_loss");
    set(sim_energy_breed_needed, "simulation", "energy_breed_needed");
    set(sim_energy_breed_loss, "simulation", "energy_breed_loss");

    set(grass_spawn_time, "grass", "spawn_time");
    set(grass_spawn_count, "grass", "spawn_count");
    set(grass_nutrition_value, "grass", "nutrition_value");

    set(chicken_spawn_count, "chicken", "spawn_count");
    set(chicken_sensor_range, "chicken", "sensor_range");
    set(chicken_nutrition_value, "chicken", "nutrition_value");
    set(chicken_hunger_start, "chicken", "hunger_start");
    set(chicken_hunger_stop, "chicken", "hunger_stop");

    set(wolf_spawn_count, "wolf", "spawn_count");
    set(wolf_sensor_range, "wolf", "sensor_range");
    set(wolf_hunger_start, "wolf", "hunger_start");
    set(wolf_hunger_stop, "wolf", "hunger_stop");

    return valid;
}
