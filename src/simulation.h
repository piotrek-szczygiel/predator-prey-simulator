#pragma once
#include <cstdint>
#include <random>
#include <vector>
#include "agent.h"
#include "config.h"
#include "map.h"

struct Path {
    Vec2 step;
    Agent* agent;
    int dist;
};

class Simulation {
   public:
    explicit Simulation(const Config& config)
        : m_config(config),
          m_map(config.sim_width, config.sim_height, config.sim_chunk_width, config.sim_chunk_height),
          m_mt19937(m_random_device()) {
        reset();
    };

    void reset();
    void update();

    Tick ticks() const { return m_tick; }

    Agent* at(int x, int y) const { return m_grid[y][x]; }
    int count(AgentType type) const;

#ifndef NDEBUG
    void draw_debug();
#endif

   private:
    Tick m_tick = 0;

    const Config& m_config;

    Tick m_last_cabbages_spawn = 0;

    Map m_map;
    std::vector<std::vector<Agent*>> m_grid{};

    std::random_device m_random_device{};
    std::mt19937 m_mt19937;

    int rand_int(int min, int max) {
        if (min > max) {
            int tmp = max;
            max = min;
            min = tmp;
        }
        return ((int)m_mt19937() % (abs(max - min) + 1) + min);
    }

    void add_agent(AgentType type, int x, int y);
    void move_agent(Agent* agent, int x, int y);
    bool move_agent_if_empty(Agent* agent, int x, int y);
    void move_agent_random(Agent* agent);
    void move_agent_around(Agent* agent, int x, int y);

    bool out_of_map(int x, int y) const {
        return x < 0 || y < 0 || x >= m_config.sim_width || y >= m_config.sim_height;
    }
    bool empty(int x, int y) const { return !out_of_map(x, y) && (m_grid[y][x] == nullptr || m_grid[y][x]->is_dead()); }

    void spawn_random_agents(AgentType type, int count);
    Agent* spawn_around(AgentType type, int x, int y);

    void update_chicken(Agent* chicken);
    void update_wolf(Agent* wolf);

    Path get_path_to_nearest(Agent* from, AgentType to, int sensor_range, int to_min_energy = 0);

#ifndef NDEBUG
    struct DebugLine {
        Agent* from;
        Agent* to;
    };
    std::vector<DebugLine> m_debug_lines{};

    struct DebugBreed {
        Agent* mom;
        Agent* dad;
        Agent* kid;
    };
    std::vector<DebugBreed> m_debug_breeds{};
#endif
};