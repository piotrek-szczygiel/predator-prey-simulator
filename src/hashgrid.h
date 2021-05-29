#pragma once
#include <array>
#include <cassert>
#include <list>
#include "agent.h"

struct ChunkUpdate {
    int old_chunk;
    int new_chunk;
    Agent* agent;
};

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

    Agent* add(int x, int y, AgentType type) { return &chunks.at(get_chunk(x, y)).emplace_back(x, y, type); }

    void move(Agent* agent, int x, int y) {
        int old_chunk = get_chunk(agent->x, agent->y);
        int new_chunk = get_chunk(x, y);

        if (old_chunk != new_chunk) {
            m_chunk_updates.push_back({old_chunk, new_chunk, agent});
        }

        agent->x = x;
        agent->y = y;
    }

    int count(AgentType type) const {
        int count = 0;
        for (const auto& chunk : chunks) {
            for (const auto& agent : chunk) {
                if (agent.type == type) ++count;
            }
        }
        return count;
    }

    std::vector<Agent*> update_chunks() {
        std::vector<Agent*> updates;
        for (const auto& update : m_chunk_updates) {
            auto& old_chunk = chunks.at(update.old_chunk);
            auto& new_chunk = chunks.at(update.new_chunk);
            for (auto it = old_chunk.begin(); it != old_chunk.end(); ++it) {
                if (&*it == update.agent) {
                    new_chunk.splice(new_chunk.end(), old_chunk, it);
                    updates.push_back(&new_chunk.back());
                    break;
                }
            }
        }

        m_chunk_updates.clear();
        return updates;
    }

   private:
    int m_map_width;
    int m_map_height;

    int m_chunk_x_count;
    int m_chunk_y_count;

    int m_chunk_width;
    int m_chunk_height;

    int m_count;

    int get_chunk(int x, int y) {
        x /= m_chunk_width;
        y /= m_chunk_height;
        int chunk = y * m_chunk_x_count + x;
        return chunk;
    }

    std::vector<ChunkUpdate> m_chunk_updates;
};