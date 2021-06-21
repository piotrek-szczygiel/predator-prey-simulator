#pragma once
#include "agent.h"
#include "util.h"

class Grid {
   public:
    Grid(Vec2 size) : m_size(size) { clear(); }

    void clear() {
        m_grid.clear();
        m_grid.resize(m_size.x * m_size.y, nullptr);
    }

    Agent *at(Vec2 pos) const { return m_grid[pos.y * m_size.x + pos.x]; }
    Agent *&at_mut(Vec2 pos) { return m_grid[pos.y * m_size.x + pos.x]; }

   private:
    Vec2 m_size;

    std::vector<Agent *> m_grid;
};
