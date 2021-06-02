#pragma once
#include <chrono>
#include <cstdint>
#include <random>
#include <vector>
#include "agent.h"
#include "config.h"
#include "map.h"
#include "pathfinder.h"

struct Path {
    Vec2 step;
    Agent* agent;
    int dist;
};

struct DebugLine {
    Agent* from;
    Agent* to;
};

struct DebugBreed {
    Agent* mom;
    Agent* dad;
    Agent* kid;
};

class Simulation {
   public:
    explicit Simulation(const Config& config)
        : m_config(config),
          m_width(config.sim_width),
          m_height(config.sim_height),
          m_map(config.sim_width, config.sim_height, config.sim_chunk_width, config.sim_chunk_height),
          m_seed(m_random_device()),
          m_mt19937(m_seed),
          m_pathfinder(new Pathfinder(m_width, m_height)) {
        reset();
    };

    void reset();
    void update();

    int width() const { return m_width; }
    int height() const { return m_height; }

    Tick ticks() const { return m_tick; }
    double update_time() const { return m_update_times.back(); }
    double avg_update_time() const { return m_avg_update_time; }

    unsigned int seed() const { return m_seed; }

    Agent* at(int x, int y) const { return m_grid[y][x]; }
    int count(AgentType type) const;

    std::vector<std::list<Agent>>& chunks() { return m_map.chunks(); }
    int chunk_x_count() const { return m_map.m_chunk_x_count; }
    int chunk_y_count() const { return m_map.m_chunk_y_count; }
    int chunk_width() const { return m_map.m_chunk_width; }
    int chunk_height() const { return m_map.m_chunk_height; }

    const std::vector<DebugLine>& debug_lines() const { return m_debug_lines; }
    const std::vector<DebugBreed>& debug_breeds() const { return m_debug_breeds; }

   private:
    int m_width;
    int m_height;

    Tick m_tick = 0;
    std::vector<double> m_update_times{};
    double m_avg_update_time = 0.0;

    const Config& m_config;

    Tick m_last_grass_spawn = 0;

    Map m_map;
    std::vector<std::vector<Agent*>> m_grid{};
    Pathfinder* m_pathfinder;

    std::random_device m_random_device{};
    unsigned int m_seed;
    std::mt19937 m_mt19937;

    int random(int min, int max) { return std::uniform_int_distribution(min, max)(m_mt19937); }
    Vec2 random_position() { return {random(0, m_width - 1), random(0, m_height - 1)}; }

    void add_agent(AgentType type, int x, int y);
    void move_agent(Agent* agent, int x, int y);
    bool move_agent_if_empty(Agent* agent, int x, int y);
    void move_agent_random(Agent* agent);
    void move_agent_around(Agent* agent, int x, int y);

    bool out_of_map(int x, int y) const { return x < 0 || y < 0 || x >= m_width || y >= m_height; }
    bool empty(int x, int y) const { return !out_of_map(x, y) && (m_grid[y][x] == nullptr || m_grid[y][x]->is_dead()); }

    void spawn_random_agents(AgentType type, int count);
    Agent* spawn_around(AgentType type, Vec2 p);

    void update_chicken(Agent* chicken);
    void update_wolf(Agent* wolf);

    Path get_path_to_nearest(Agent* from, AgentType to, int sensor_range, int to_min_energy = 0);

    std::vector<DebugLine> m_debug_lines{};
    std::vector<DebugBreed> m_debug_breeds{};
};