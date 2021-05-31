#pragma once
#include <iostream>
#include "ini.h"

class Config {
   public:
    bool load() {
        auto ini = INIReader("config.ini");
        if (ini.ParseError() != 0) {
            return false;
        }

        valid = true;

        tile_size = 16.0f;

        window_width = get(ini, "window", "width");
        window_height = get(ini, "window", "height");
        window_fps = get(ini, "window", "fps");

        sim_width = get(ini, "simulation", "width");
        sim_height = get(ini, "simulation", "height");
        sim_chunk_width = get(ini, "simulation", "chunk_width");
        sim_chunk_height = get(ini, "simulation", "chunk_height");

        sim_energy_start = get(ini, "simulation", "energy_start");
        sim_energy_breed_needed = get(ini, "simulation", "energy_breed_needed");
        sim_energy_breed_loss = get(ini, "simulation", "energy_breed_loss");
        sim_energy_tick_loss = get(ini, "simulation", "energy_tick_loss");

        cabbage_spawn_time = get(ini, "cabbage", "spawn_time");
        cabbage_spawn_count = get(ini, "cabbage", "spawn_count");
        cabbage_nutrition_value = get(ini, "cabbage", "nutrition_value");

        chicken_spawn_count = get(ini, "chicken", "spawn_count");
        chicken_sensor_range = get(ini, "chicken", "sensor_range");
        chicken_nutrition_value = get(ini, "chicken", "nutrition_value");
        chicken_hungry_start = get(ini, "chicken", "hungry_start");
        chicken_hungry_stop = get(ini, "chicken", "hungry_stop");

        wolf_spawn_count = get(ini, "wolf", "spawn_count");
        wolf_sensor_range = get(ini, "wolf", "sensor_range");

        return valid;
    }

    float tile_size;

    int window_width;
    int window_height;
    int window_fps;

    int sim_width;
    int sim_height;
    int sim_chunk_width;
    int sim_chunk_height;

    int sim_energy_start;
    int sim_energy_breed_needed;
    int sim_energy_breed_loss;
    int sim_energy_tick_loss;

    int cabbage_spawn_time;
    int cabbage_spawn_count;
    int cabbage_nutrition_value;

    int chicken_spawn_count;
    int chicken_sensor_range;
    int chicken_nutrition_value;
    int chicken_hungry_start;
    int chicken_hungry_stop;

    int wolf_spawn_count;
    int wolf_sensor_range;

    bool valid;

   private:
    int get(const INIReader& ini, const std::string& section, const std::string& name) {
        std::string str_value = ini.Get(section, name, "");
        const char* value = str_value.c_str();
        char* end;
        long n = strtol(value, &end, 0);
        if (end > value) return n;
        std::cerr << "invalid value for [" << section << "] " << name << ": '" << str_value << "'\n";
        valid = false;
        return -1;
    }
};
