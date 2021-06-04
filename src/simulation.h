#pragma once
#include <chrono>
#include <cstdint>
#include <random>
#include <vector>
#include "agent.h"
#include "config.h"
#include "grid.h"
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
          m_size({config.sim_width, config.sim_height}),
          m_map(m_size, {config.sim_chunk_width, config.sim_chunk_height}),
          m_grid(m_size),
          m_random_device(),
          m_seed(config.seed_manual ? seed_from_str(config.seed) : m_random_device()),
          m_mt19937(m_seed),
          m_pathfinder(Pathfinder(m_size)) {
        reset();
    };

    void reset();
    void update();

    Vec2 size() { return m_size; }
    Tick ticks() const { return m_tick; }
    double update_time() const { return m_update_times.back(); }
    double avg_update_time() const { return m_avg_update_time; }

    unsigned int seed() const { return m_seed; }

    int count(AgentType type) const;
    inline bool out_of_map(Vec2 pos) const { return pos.x < 0 || pos.y < 0 || pos.x >= m_size.x || pos.y >= m_size.y; }

    const Agent* at(Vec2 pos) const { return m_grid.at(pos); }

    std::vector<std::list<Agent>>& chunks() { return m_map.chunks(); }
    Vec2 chunk_count() const { return m_map.chunk_count(); }
    Vec2 chunk_size() const { return m_map.chunk_size(); }

    const std::vector<DebugLine>& debug_lines() const { return m_debug_lines; }
    const std::vector<DebugBreed>& debug_breeds() const { return m_debug_breeds; }

   private:
    Vec2 m_size;

    Tick m_tick = 0;
    std::vector<double> m_update_times{};
    double m_avg_update_time = 0.0;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_last_average{};

    const Config& m_config;

    Tick m_last_grass_spawn = 0;

    Map m_map;
    Grid m_grid;
    Pathfinder m_pathfinder;

    std::random_device m_random_device;
    uint32_t m_seed;
    std::mt19937 m_mt19937;

    std::vector<Vec2> m_possible_random_moves{{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    std::vector<Vec2> m_possible_spawn_offsets{{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 0},
                                               {0, 1},   {1, -1}, {1, 0},  {1, 1}};

    int random(int min, int max) { return std::uniform_int_distribution(min, max)(m_mt19937); }
    Vec2 random_position() { return {random(0, m_size.x - 1), random(0, m_size.y - 1)}; }

    void add_agent(AgentType type, Vec2 pos);
    void move_agent(Agent* agent, Vec2 pos);
    bool move_agent_if_empty(Agent* agent, Vec2 pos);
    void move_agent_around(Agent* agent, Vec2 pos);
    void move_agent_random(Agent* agent);

    inline bool empty(Vec2 pos) const { return !out_of_map(pos) && (!m_grid.at(pos) || m_grid.at(pos)->is_dead()); }

    void spawn_random_agents(AgentType type, int count);
    Agent* spawn_around(AgentType type, Vec2 p);

    void update_chicken(Agent* chicken);
    void update_wolf(Agent* wolf);

    Path get_path_to_nearest(Agent* from, AgentType to, int sensor_range, int to_min_energy = 0);

    std::vector<DebugLine> m_debug_lines{};
    std::vector<DebugBreed> m_debug_breeds{};
};