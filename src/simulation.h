#pragma once
#include <cstdint>
#include <vector>
#include "agent.h"
#include "config.h"
#include "map.h"

using Tick = uint64_t;

class Simulation {
   public:
    Simulation(const Config& config)
        : m_config(config),
          m_map(config.sim_width, config.sim_height, config.sim_chunk_width, config.sim_chunk_height) {
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

    void add_agent(AgentType type, int x, int y);
    void move_agent(Agent* agent, int x, int y);

    bool out_of_map(int x, int y) const {
        return x < 0 || y < 0 || x >= m_config.sim_width || y >= m_config.sim_height;
    }

    void spawn_random_agents(AgentType type, int count);

    void update_chicken(Agent* chicken);
    void update_wolf(Agent* wolf);

    Vec2 get_step_to(Agent* from, AgentType to, int sensor_range);

#ifndef NDEBUG
    struct DebugLine {
        Agent* from;
        Agent* to;
    };
    std::vector<DebugLine> m_debug_lines{};
#endif
};