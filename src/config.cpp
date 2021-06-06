#include "config.h"
#include <iomanip>

bool Config::write() {
    for (const auto& link : m_links) {
        std::string v;
        if (link.type == LinkType::Int) {
            v = std::to_string(*link.out_int);
        } else if (link.type == LinkType::Float) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << *link.out_float;
            v = oss.str();
        } else if (link.type == LinkType::Bool) {
            v = *link.out_bool ? "true" : "false";
        }

        m_ini[link.section][link.name] = v;
    }
    m_ini["seed"]["seed"] = seed;
    return m_file.write(m_ini, false);
}

std::string Config::get(const char* section, const char* name) {
    std::string value = m_ini.get(section).get(name);
    if (value.empty()) {
        fprintf(stderr, "unable to find property '[%s] %s' in %s\n", section, name, m_filename);
        valid = false;
    }
    return value;
}

void Config::set(int& output, const char* section, const char* name) {
    auto str_value = get(section, name);
    if (str_value.empty()) return;

    const char* value = str_value.c_str();
    char* end;
    long result = std::strtol(value, &end, 10);

    if (end <= value) {
        fprintf(stderr, "invalid integer '%s' for property '[%s] %s' in %s\n", value, section, name, m_filename);
        valid = false;
        return;
    }

    output = result;

    Link link{};
    link.type = LinkType::Int;
    link.out_int = &output;
    link.section = section;
    link.name = name;
    m_links.push_back(link);
}

void Config::set(float& output, const char* section, const char* name) {
    std::string str_value = m_ini.get(section).get(name);
    if (str_value.empty()) return;

    const char* value = str_value.c_str();
    char* end;
    float result = std::strtof(value, &end);

    if (end <= value) {
        fprintf(stderr, "invalid float '%s' for property '[%s] %s' in %s\n", value, section, name, m_filename);
        valid = false;
        return;
    }

    output = result;

    Link link{};
    link.type = LinkType::Float;
    link.out_float = &output;
    link.section = section;
    link.name = name;
    m_links.push_back(link);
}

void Config::set(bool& output, const char* section, const char* name) {
    auto str_value = get(section, name);
    if (str_value.empty()) return;

    output = str_value == "true";

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
    set(window_zoom, "window", "zoom");
    set(window_fps, "window", "fps");
    set(window_help, "window", "help");

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

    set(grass_spawn_count, "grass", "spawn_count");
    set(grass_nutritional_value, "grass", "nutritional_value");

    set(chicken_spawn_count, "chicken", "spawn_count");
    set(chicken_sensor_range, "chicken", "sensor_range");
    set(chicken_energy_start, "chicken", "energy_start");
    set(chicken_energy_loss, "chicken", "energy_loss");
    set(chicken_hunger_start, "chicken", "hunger_start");
    set(chicken_hunger_stop, "chicken", "hunger_stop");
    set(chicken_breed_cost, "chicken", "breed_cost");
    set(chicken_nutritional_value, "chicken", "nutritional_value");

    set(wolf_spawn_count, "wolf", "spawn_count");
    set(wolf_sensor_range, "wolf", "sensor_range");
    set(wolf_energy_start, "wolf", "energy_start");
    set(wolf_energy_loss, "wolf", "energy_loss");
    set(wolf_hunger_start, "wolf", "hunger_start");
    set(wolf_hunger_stop, "wolf", "hunger_stop");
    set(wolf_breed_cost, "wolf", "breed_cost");

    return valid;
}