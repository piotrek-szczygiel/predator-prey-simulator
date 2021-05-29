#include "simulation.h"
#include "util.h"

void Simulation::reset() {
    m_map.clear();
    m_ptr_grid.clear();
    m_ptr_grid.assign((size_t)m_width * m_height, nullptr);

    spawn_random_agents(AgentType::Chicken, CHICKEN_SPAWN_COUNT);
    spawn_random_agents(AgentType::Wolf, WOLF_SPAWN_COUNT);
    spawn_random_agents(AgentType::Cabbage, CABBAGE_SPAWN_COUNT);
}

void Simulation::update() {
    if (m_tick - m_last_cabbages_spawn > CABBAGE_SPAWN_TIME) {
        spawn_random_agents(AgentType::Cabbage, CABBAGE_SPAWN_COUNT);
        m_last_cabbages_spawn = m_tick;
    }

    for (auto& chunk : m_map.chunks()) {
        for (auto agent = chunk.begin(); agent != chunk.end();) {
            agent->energy -= ENERGY_TICK_LOSS;

            if (agent->is_dead()) {
                at(agent->x, agent->y) = nullptr;
                agent = chunk.erase(agent);
                continue;
            }

            switch (agent->type) {
                case AgentType::Chicken: update_chicken(&*agent); break;
                default: break;
            }

            ++agent;
        }
    }

    for (Agent* agent : m_map.update_chunks()) {
        at(agent->x, agent->y) = agent;
    }

    ++m_tick;
}

AgentType Simulation::type_at(int x, int y) const {
    Agent* agent = m_ptr_grid.at(id(x, y));
    return agent ? agent->type : AgentType::None;
}

int Simulation::count(AgentType type) const {
    return m_map.count(type);
}

void Simulation::add_agent(int x, int y, AgentType type) {
    Agent* agent = m_map.add(x, y, type);
    at(x, y) = agent;
}

void Simulation::move_agent(Agent* agent, int x, int y) {
    int old_x = agent->x;
    int old_y = agent->y;
    m_map.move(agent, x, y);
    at(x, y) = agent;
    at(old_x, old_y) = nullptr;
}

void Simulation::spawn_random_agents(AgentType type, int count) {
    for (int i = 0; i < count; ++i) {
        int x = random(0, m_width - 1);
        int y = random(0, m_height - 1);

        if (!at(x, y)) {
            add_agent(x, y, type);
        }
    }
}

void Simulation::update_chicken(Agent* chicken) {
    int xx = chicken->x;
    int yy = chicken->y;
    do {
        xx += random(-1, 1);
        yy += random(-1, 1);
    } while (out_of_map(xx, yy));

    if (!at(xx, yy)) move_agent(chicken, xx, yy);
}

#ifdef _DEBUG
#include <raylib.h>

void draw_line(int x, int y, int xx, int yy, Color color) {
    color.a = 128;
    DrawLine(x * 16 + 8, y * 16 + 8, xx * 16 + 8, yy * 16 + 8, color);
}

void Simulation::draw_debug() {
    for (int i = 0; i < m_map.chunks().size(); ++i) {
        DrawRectangleLines((i % m_map.m_chunk_x_count) * m_map.m_chunk_width * 16.0f,
                           (i / m_map.m_chunk_y_count) * m_map.m_chunk_height * 16.0f, m_map.m_chunk_width * 16.0f,
                           m_map.m_chunk_height * 16.0f, RAYWHITE);

        const auto& chunk = m_map.chunks().at(i);
        for (const auto& agent : chunk) {
            if (agent.type == AgentType::Chicken) {
                for (const auto& target : m_map.get_nearby_to(&agent)) {
                    Color c = (target->type == AgentType::Chicken ? BLUE : RED);
                    draw_line(agent.x, agent.y, target->x, target->y, c);
                }
            }
        }
    }
}
#endif
