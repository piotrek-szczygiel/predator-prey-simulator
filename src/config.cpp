#include "config.h"

bool Config::write() {
    for (const auto& link : m_links) {
        m_ini[link.section][link.name] =
            link.type == LinkType::Int ? std::to_string(*link.out_int) : (*link.out_bool ? "true" : "false");
    }
    m_ini["seed"]["seed"] = seed;
    return m_file.write(m_ini, false);
}

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

    Link link{};
    link.type = LinkType::Int;
    link.out_int = &output;
    link.section = section;
    link.name = name;
    m_links.push_back(link);
}

void Config::set(bool& output, const char* section, const char* name) {
    output = m_ini.get(section).get(name) == "true";

    Link link{};
    link.type = LinkType::Bool;
    link.out_bool = &output;
    link.section = section;
    link.name = name;
    m_links.push_back(link);
}

bool Config::load() {
    if (!m_file.read(m_ini)) return false;

    valid = true;

    tile_size = 16.0f;

    set(window_width, "window", "width");
    set(window_height, "window", "height");
    set(window_style, "window", "style");
    set(window_maximized, "window", "maximized");
    set(window_fps, "window", "fps");

    set(runtime_tick_time_ms, "runtime", "tick_time_ms");
    set(runtime_manual_stepping, "runtime", "manual_stepping");
    set(runtime_debug_draw, "runtime", "debug_draw");

    set(seed_manual, "seed", "manual");
    std::snprintf(seed, SEED_SIZE, "%s", m_ini.get("seed").get("seed").c_str());

    set(genes_max_offsprings, "genes", "max_offsprings");
    set(genes_max_sensor_range, "genes", "max_sensor_range");

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