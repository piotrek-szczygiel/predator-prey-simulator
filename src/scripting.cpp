#include "scripting.h"
#include "simulation.h"

#include <memory>

bool Scripting::init() {
    m_lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::jit);
    m_lua["print"] = [](const char* text) { fprintf(stderr, "script: %s\n", text); };
    m_lua["error_handler"] = [](const char* text) { return text; };
    sol::protected_function_result result = m_lua.safe_script("return 2 + 2");
    if (!result.valid()) return false;
    return 4 == result.get<int>();
}

bool Scripting::load(const std::string& path, Simulation& sim) {
    m_valid = false;

    m_lua["Type"] =
        m_lua.create_table_with("Chicken", AgentType::Chicken, "Wolf", AgentType::Wolf, "Grass", AgentType::Grass);

    auto genes_t = m_lua.new_usertype<AgentGenes>("Genes");
    genes_t["offsprings"] = &AgentGenes::offsprings;
    genes_t["sensorRange"] = &AgentGenes::sensor_range;

    auto vec_t = m_lua.new_usertype<Vec2>("Vec2");
    vec_t["x"] = &Vec2::x;
    vec_t["y"] = &Vec2::y;
    vec_t.set_function(sol::meta_function::unary_minus, [](Vec2 v) { return Vec2{-v.x, -v.y}; });

    auto agent_t = m_lua.new_usertype<Agent>("Agent");
    agent_t["energy"] = &Agent::energy;
    agent_t["genes"] = sol::readonly(&Agent::genes);
    agent_t["hungry"] = &Agent::hungry;
    agent_t["waitTime"] = sol::readonly(&Agent::last_update);
    agent_t["position"] = sol::property([](Agent* a) { return std::make_pair<>(a->pos.x, a->pos.y); });
    agent_t["lastMove"] = sol::property([&](Agent* a) { return sim.ticks() - a->last_update; });

    agent_t["isDead"] = &Agent::is_dead;
    agent_t["kill"] = &Agent::kill;
    agent_t["move"] = [&](Agent* a, int x, int y) { return sim.move_agent_around(a, {x, y}); };
    agent_t["moveStep"] = [&](Agent* a, Vec2 s) { return sim.move_agent_around(a, {a->pos.x + s.x, a->pos.y + s.y}); };
    agent_t["resetRandom"] = [](Agent* a) { a->random_direction = {}; };
    agent_t["moveRandom"] = [&](Agent* a, int d) { return sim.move_agent_random(a, d); };
    agent_t["moveRandomAnywhere"] = [&](Agent* a) { return sim.move_agent_random(a, -1); };
    agent_t["pathToNearest"] = [&](Agent* a, AgentType t) { return sim.get_path_to_nearest(a, t); };
    agent_t["pathToNearestFed"] = [&](Agent* a, AgentType t) { return sim.get_path_to_nearest(a, t, true); };
    agent_t["breed"] = [&](Agent* a, Agent* b) { return sim.breed(a, b); };

    const Config& c = sim.m_config;

    sol::table c_genes =
        m_lua.create_table_with("maxOffsprings", c.genes_max_offsprings, "maxSensorRange", c.genes_max_sensor_range);

    sol::table c_sim = m_lua.create_table_with("width", c.sim_width, "height", c.sim_height, "chunkWidth",
                                               c.sim_chunk_width, "chunkHeight", c.sim_chunk_height);

    sol::table c_grass =
        m_lua.create_table_with("spawnCount", c.grass_spawn_count, "nutritionalValue", c.grass_nutritional_value);

    sol::table c_chicken =
        m_lua.create_table_with("spawnCount", c.chicken_spawn_count, "sensorRange", c.chicken_sensor_range,
                                "energyStart", c.chicken_energy_start, "energyLoss", c.chicken_energy_loss,
                                "hungerStart", c.chicken_hunger_start, "hungerStop", c.chicken_hunger_stop, "breedCost",
                                c.chicken_breed_cost, "nutritionalValue", c.chicken_nutritional_value);

    sol::table c_wolf =
        m_lua.create_table_with("spawnCount", c.wolf_spawn_count, "sensorRange", c.wolf_sensor_range, "energyStart",
                                c.wolf_energy_start, "energyLoss", c.wolf_energy_loss, "hungerStart",
                                c.wolf_hunger_start, "hungerStop", c.wolf_hunger_stop, "breedCost", c.wolf_breed_cost);

    m_lua["Config"] =
        m_lua.create_table_with("genes", c_genes, "sim", c_sim, "grass", c_grass, "chicken", c_chicken, "wolf", c_wolf);

    auto result = m_lua.safe_script_file(path, sol::script_pass_on_error);
    if (!result.valid()) {
        sol::error err = result;
        fprintf(stderr, "\nerror while executing script %s!\n%s\n", path.c_str(), err.what());
        return false;
    }

    m_update_chicken = m_lua["update_chicken"];
    m_update_chicken.error_handler = m_lua["error_handler"];

    m_update_wolf = m_lua["update_wolf"];
    m_update_wolf.error_handler = m_lua["error_handler"];

    if (!m_update_chicken.valid()) {
        fprintf(stderr, "unable to find function update_chicken(chicken)\n");
        return false;
    }

    if (!m_update_wolf.valid()) {
        fprintf(stderr, "unable to find function update_wolf(wolf)\n");
        return false;
    }

    m_valid = true;
    return true;
}

void Scripting::update_chicken(Agent* chicken) {
    if (m_valid) {
        sol::protected_function_result result = m_update_chicken(chicken);
        if (!result.valid()) {
            sol::error err = result;
            fprintf(stderr, "\nerror while executing update_chicken\n%s\n", err.what());
        }
    }
}

void Scripting::update_wolf(Agent* wolf) {
    if (m_valid) {
        sol::protected_function_result result = m_update_wolf(wolf);
        if (!result.valid()) {
            sol::error err = result;
            fprintf(stderr, "\nerror while executing update_wolf\n%s\n", err.what());
        }
    }
}