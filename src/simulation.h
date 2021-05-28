#pragma once
#include <vector>
#include "agent.h"

class Simulation {
   public:
    Simulation(int width, int height) : m_width(width), m_height(height) { reset(); };

    void reset();
    void update(float dt);

    int width() const { return m_width; }
    int height() const { return m_height; }

    AgentType get_agent_type(int x, int y) const { return m_map.at((std::size_t)y * m_width + x).type; }

   private:
    int m_width;
    int m_height;

    std::vector<Agent> m_map;

    float m_last_cabbages_spawn;

    Agent& at(int x, int y) { return m_map.at((std::size_t)y * m_width + x); }
    void spawn_cabbages(float time);
};