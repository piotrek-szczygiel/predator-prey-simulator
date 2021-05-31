#include "map.h"

std::vector<Agent*> Map::get_nearby_to(const Agent* agent) {
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
void Map::move(Agent* agent, int x, int y) {
    int old_chunk = get_chunk(agent->x, agent->y);
    int new_chunk = get_chunk(x, y);

    if (old_chunk != new_chunk) {
        m_chunk_updates.push_back({old_chunk, new_chunk, agent});
    }

    agent->x = x;
    agent->y = y;
}
int Map::count(AgentType type) const {
    int count = 0;
    for (const auto& chunk : m_chunks) {
        for (const auto& agent : chunk) {
            if (agent.type == type) ++count;
        }
    }
    return count;
}
std::vector<Agent*> Map::update_chunks() {
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
int Map::get_chunk(int x, int y) const {
    x /= m_chunk_width;
    y /= m_chunk_height;
    int chunk = y * m_chunk_x_count + x;
    assert(chunk < m_chunks.size());
    return chunk;
}
