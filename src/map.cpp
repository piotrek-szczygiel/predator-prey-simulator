#include "map.h"

std::vector<Agent*> Map::get_nearby_to(const Agent* agent) {
    int c = get_chunk(agent->pos);
    std::vector<int> chunks{c};

    int x = agent->pos.x % m_chunk_size.x;
    int y = agent->pos.y % m_chunk_size.y;

    int c_x = m_chunk_count.x;

    if (x < m_chunk_size.x / 2) {
        if (y < m_chunk_size.y / 2) {  // left-top
            if (c % c_x != 0) {
                chunks.push_back(c - 1);
                chunks.push_back(c - c_x - 1);
            }
            chunks.push_back(c - c_x);
        } else {  // left-bottom
            if (c % c_x != 0) {
                chunks.push_back(c - 1);
                chunks.push_back(c + c_x - 1);
            }
            chunks.push_back(c + c_x);
        }
    } else {
        if (y < m_chunk_size.y / 2) {  // right-top
            if ((c + 1) % c_x != 0) {
                chunks.push_back(c + 1);
                chunks.push_back(c - c_x + 1);
            }
            chunks.push_back(c - c_x);
        } else {  // right-bottom
            if ((c + 1) % c_x != 0) {
                chunks.push_back(c + 1);
                chunks.push_back(c + c_x + 1);
            }
            chunks.push_back(c + c_x);
        }
    }

    std::vector<Agent*> agents;
    agents.reserve(m_reserve);
    for (auto& chunk : chunks) {
        if (chunk < 0 || chunk >= m_chunks.size()) continue;
        for (auto& a : m_chunks.at(chunk)) {
            if (&a != agent && !a.is_dead()) agents.push_back(&a);
        }
    }
    return agents;
}

void Map::move(Agent* agent, Vec2 pos) {
    int old_chunk = get_chunk(agent->pos);
    int new_chunk = get_chunk(pos);

    if (old_chunk != new_chunk) {
        m_chunk_updates.push_back({old_chunk, new_chunk, agent});
    }

    agent->pos = pos;
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

int Map::get_chunk(Vec2 pos) const {
    pos.x /= m_chunk_size.x;
    pos.y /= m_chunk_size.y;
    return pos.y * m_chunk_count.x + pos.x;
}

void Map::clear() {
    m_chunks.clear();
    m_chunks.assign(m_chunk_count.x * m_chunk_count.y, {});
}

void Map::remove_dead() {
    for (auto& chunk : m_chunks) chunk.remove_if([](const auto& agent) { return agent.is_dead(); });
}

Agent* Map::add(AgentType type, Vec2 pos, int energy, Tick last_update) {
    return &m_chunks.at(get_chunk(pos)).emplace_back(type, pos, energy, last_update);
}