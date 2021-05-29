#pragma once
#include <algorithm>
#include <unordered_map>
#include <vector>
#include "agent.h"
#include "map.h"

using Tick = uint64_t;

class Simulation {
   public:
    Simulation(int width, int height) : m_width(width), m_height(height), m_map(width, height, width / 5, height / 5) {
        reset();
    };

    void reset();
    void update();

    int width() const { return m_width; }
    int height() const { return m_height; }
    Tick ticks() const { return m_tick; }

    AgentType type_at(int x, int y) const;
    int count(AgentType type) const;

#ifdef _DEBUG
    void draw_debug();
#endif

   private:
    Tick m_tick = 0;

    int m_width;
    int m_height;

    Tick m_last_cabbages_spawn = 0;

    Map m_map;
    std::vector<Agent*> m_ptr_grid;

    int id(int x, int y) const { return y * m_width + x; }
    Agent*& at(int x, int y) { return m_ptr_grid.at(id(x, y)); }
    void add_agent(int x, int y, AgentType type);
    void move_agent(Agent* agent, int x, int y);

    bool out_of_map(int x, int y) const { return x < 0 || y < 0 || x >= m_width || y >= m_height; }

    void spawn_random_agents(AgentType type, int count);
    void update_chicken(Agent* chicken);
};