#pragma once
#include <raylib.h>
#include <array>
#include <cmath>
#include <list>
#include <vector>
#include "agent.h"

struct ChunkUpdate {
    int old_chunk;
    int new_chunk;
    Agent* agent;
};

class Map {
   public:
    Map(int map_width, int map_height, int chunk_width, int chunk_height)
        : m_chunk_width(chunk_width), m_chunk_height(chunk_height) {
        m_chunk_x_count = std::ceil((double)map_width / chunk_width);
        m_chunk_y_count = std::ceil((double)map_height / chunk_height);
        m_count = m_chunk_x_count * m_chunk_y_count;
        clear();
    }

    std::vector<std::list<Agent>>& chunks() { return m_chunks; }

    void clear() {
        m_chunks.clear();
        m_chunks.assign(m_count, {});
    }

    void remove_dead() {
        for (auto& chunk : m_chunks) chunk.remove_if([](const auto& agent) { return agent.is_dead(); });
    }

    Agent* add(AgentType type, int x, int y, int energy, Tick last_update) {
        return &m_chunks.at(get_chunk(x, y)).emplace_back(type, x, y, energy, last_update);
    }

    std::vector<Agent*> get_nearby_to(const Agent* agent);
    void move(Agent* agent, int x, int y);
    int count(AgentType type) const;
    std::vector<Agent*> update_chunks();

    // public for debugging purposes
    int m_chunk_x_count;
    int m_chunk_y_count;

    int m_chunk_width;
    int m_chunk_height;

   private:
    std::vector<std::list<Agent>> m_chunks{};
    int m_count;

    int m_reserve = m_chunk_width * m_chunk_height;

    int get_chunk(int x, int y) const;
    std::vector<ChunkUpdate> m_chunk_updates{};
};