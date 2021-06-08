#include "scripting.h"
#include <memory>
#include "simulation.h"

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
    agent_t["position"] = sol::property([](Agent* agent) { return std::make_pair<>(agent->pos.x, agent->pos.y); });
    agent_t["lastMove"] = sol::property([&](Agent* agent) { return sim.ticks() - agent->last_update; });

    agent_t["isDead"] = &Agent::is_dead;
    agent_t["kill"] = &Agent::kill;
    agent_t["move"] = [&](Agent* agent, int x, int y) { return sim.move_agent_around(agent, {x, y}); };
    agent_t["moveStep"] = [&](Agent* agent, Vec2 step) {
        return sim.move_agent_around(agent, {agent->pos.x + step.x, agent->pos.y + step.y});
    };
    agent_t["resetRandom"] = [](Agent* agent) { agent->random_direction = {}; };
    agent_t["moveRandom"] = [&](Agent* agent, int dist) { return sim.move_agent_random(agent, dist); };
    agent_t["moveRandomAnywhere"] = [&](Agent* agent) { return sim.move_agent_random(agent, -1); };
    agent_t["pathToNearest"] = [&](Agent* agent, AgentType type) {
        Path p = sim.get_path_to_nearest(agent, type);
        return std::make_tuple(p.agent, p.dist, p.step);
    };
    agent_t["pathToNearestFed"] = [&](Agent* agent, AgentType type) {
        Path p = sim.get_path_to_nearest(agent, type, true);
        return std::make_tuple(p.agent, p.dist, p.step);
    };
    agent_t["breed"] = [&](Agent* agent, Agent* partner) { return sim.breed(agent, partner); };

    const Config& c = sim.m_config;
    sol::table chicken_config = m_lua.create_table_with(
        "hungerStart", c.chicken_hunger_start, "hungerStop", c.chicken_hunger_stop, "sensorRange",
        c.chicken_sensor_range, "nutritionalValue", c.chicken_nutritional_value, "energyLoss", c.chicken_energy_loss);

    sol::table wolf_config = m_lua.create_table_with("hungerStart", c.wolf_hunger_start, "hungerStop",
                                                     c.wolf_hunger_stop, "energyLoss", c.wolf_energy_loss);

    sol::table grass_config = m_lua.create_table_with("nutritionalValue", c.grass_nutritional_value);

    m_lua["Config"] = m_lua.create_table_with("chicken", chicken_config, "wolf", wolf_config, "grass", grass_config);

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