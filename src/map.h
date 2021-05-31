#pragma once
#include <raylib.h>
#include <array>
#include <cassert>
#include <list>
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
        m_chunk_x_count = map_width / chunk_width;
        m_chunk_y_count = map_height / chunk_height;
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

    std::vector<Agent*> get_nearby_to(const Agent* agent) {
        int c = get_chunk(agent->x, agent->y);
        std::vector<int> chunks;
        chunks.push_back(c);

        int x = agent->x % m_chunk_width;
        int y = agent->y % m_chunk_height;

        if (x < m_chunk_width / 2) {
            if (y < m_chunk_height / 2) {  // left-top
                if (c % m_chunk_x_count != 0) {
                    chunks.push_back(c - 1);
                    chunks.push_back(c - m_chunk_x_count - 1);
                }
                chunks.push_back(c - m_chunk_x_count);
            } else {  // left-bottom
                if (c % m_chunk_x_count != 0) {
                    chunks.push_back(c - 1);
                    chunks.push_back(c + m_chunk_x_count - 1);
                }
                chunks.push_back(c + m_chunk_x_count);
            }
        } else {
            if (y < m_chunk_height / 2) {  // right-top
                if ((c + 1) % m_chunk_x_count != 0) {
                    chunks.push_back(c + 1);
                    chunks.push_back(c - m_chunk_x_count + 1);
                }
                chunks.push_back(c - m_chunk_x_count);
            } else {  // right-bottom
                if ((c + 1) % m_chunk_x_count != 0) {
                    chunks.push_back(c + 1);
                    chunks.push_back(c + m_chunk_x_count + 1);
                }
                chunks.push_back(c + m_chunk_x_count);
            }
        }

        std::vector<Agent*> agents;
        for (auto& chunk : chunks) {
            if (chunk < 0 || chunk >= m_chunks.size()) continue;
            for (auto& a : m_chunks.at(chunk)) {
                if (&a != agent && !a.is_dead()) agents.push_back(&a);
            }
        }
        return agents;
    }

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
        for (const auto& chunk : m_chunks) {
            for (const auto& agent : chunk) {
                if (agent.type == type) ++count;
            }
        }
        return count;
    }

    std::vector<Agent*> update_chunks() {
        std::vector<Agent*> updates;
        for (const auto& update : m_chunk_updates) {
            auto& old_chunk = m_chunks.at(update.old_chunk);
            auto& new_chunk = m_chunks.at(update.new_chunk);
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

    // public for debugging purposes
    int m_chunk_x_count;
    int m_chunk_y_count;

    int m_chunk_width;
    int m_chunk_height;

   private:
    std::vector<std::list<Agent>> m_chunks{};

    int m_count;

    int get_chunk(int x, int y) const {
        x /= m_chunk_width;
        y /= m_chunk_height;
        int chunk = y * m_chunk_x_count + x;
        assert(chunk < m_chunks.size());
        return chunk;
    }

    std::vector<ChunkUpdate> m_chunk_updates{};
};