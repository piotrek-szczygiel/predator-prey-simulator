#pragma once
#include <array>
#include <cassert>
#include <list>
#include "agent.h"

class Hashgrid {
   public:
    std::vector<std::list<Agent>> chunks;

    Hashgrid(int map_width, int map_height, int chunk_width, int chunk_height)
        : m_map_width(map_width), m_map_height(map_height), m_chunk_width(chunk_width), m_chunk_height(chunk_height) {
        m_chunk_x_count = map_width / chunk_width;
        m_chunk_y_count = map_height / chunk_height;
        m_count = m_chunk_x_count * m_chunk_y_count;
        clear();
    }

    void clear() {
        chunks.clear();
        chunks.assign(m_count, {});
    }

    Agent* add(int x, int y, AgentType type) { return &chunks.at(get_bucket(x, y)).emplace_back(x, y, type); }

    int count(AgentType type) const {
        int count = 0;
        for (const auto& chunk : chunks) {
            for (const auto& agent : chunk) {
                if (agent.type == type) ++count;
            }
        }
        return count;
    }

   private:
    int m_map_width;
    int m_map_height;

    int m_chunk_x_count;
    int m_chunk_y_count;

    int m_chunk_width;
    int m_chunk_height;

    int m_count;

    int get_bucket(int x, int y) {
        x /= m_chunk_width;
        y /= m_chunk_height;
        int chunk = y * m_chunk_x_count + x;
        assert(chunk < chunks.size());
        return chunk;
    }
};