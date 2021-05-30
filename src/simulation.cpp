#include "simulation.h"
#include "pathfinder.h"

void Simulation::reset() {
    m_map.clear();
    m_grid.fill({});

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
        for (auto& agent : chunk) {
            agent.energy -= ENERGY_TICK_LOSS;
            if (agent.is_dead()) {
                if (&agent == m_grid[agent.y][agent.x]) m_grid[agent.y][agent.x] = nullptr;
                continue;
            }

            switch (agent.type) {
                case AgentType::Chicken: update_chicken(&agent); break;
                default: break;
            }
        }
    }

    for (Agent* agent : m_map.update_chunks()) {
        if (!agent->is_dead()) m_grid[agent->y][agent->x] = agent;
    }

    m_map.remove_dead();
    ++m_tick;
}

AgentType Simulation::type_at(int x, int y) const {
    Agent* agent = m_grid[y][x];
    return agent ? agent->type : AgentType::None;
}

int Simulation::count(AgentType type) const {
    return m_map.count(type);
}

void Simulation::add_agent(int x, int y, AgentType type) {
    Agent* agent = m_map.add(x, y, type);
    m_grid[y][x] = agent;
}

void Simulation::move_agent(Agent* agent, int x, int y) {
    int old_x = agent->x;
    int old_y = agent->y;
    m_map.move(agent, x, y);
    m_grid[y][x] = agent;
    m_grid[old_y][old_x] = nullptr;
}

void Simulation::spawn_random_agents(AgentType type, int count) {
    for (int i = 0; i < count; ++i) {
        int x = random(0, m_width - 1);
        int y = random(0, m_height - 1);

        if (!m_grid[y][x]) {
            add_agent(x, y, type);
        }
    }
}

void Simulation::update_chicken(Agent* chicken) {
    int xx;
    int yy;

    int sensor = 10;

    const auto& others = m_map.get_nearby_to(chicken);
    Pathfinder pathfinder(sensor, m_width, m_height, chicken->x, chicken->y);

    int min_distance = INT32_MAX;
    const Agent* target = nullptr;

    for (const auto& other : others) {
        int dist = distance({chicken->x, chicken->y}, {other->x, other->y});
        if (dist <= sensor) {
            pathfinder.add_blocker({other->x, other->y});
            if (other->type == AgentType::Cabbage) {
                if (min_distance > dist) {
                    min_distance = dist;
                    target = other;
                }
            }
        }
    }

    if (target) {
        auto next_step = pathfinder.get_next_step({target->x, target->y});
        xx = chicken->x + next_step.x;
        yy = chicken->y + next_step.y;
    } else {
        do {
            xx = chicken->x + random(-1, 1);
            yy = chicken->y + random(-1, 1);
        } while (out_of_map(xx, yy));
    }

    Agent* dest = m_grid[yy][xx];
    if (!dest) {
        move_agent(chicken, xx, yy);
    } else if (dest->type == AgentType::Cabbage) {
        dest->kill();
        chicken->energy += CABBAGE_NUTRITION_VALUE;
        move_agent(chicken, xx, yy);
    }
}

#ifndef NDEBUG
#include <raylib.h>

void draw_line(int x, int y, int xx, int yy, Color color) {
    color.a = 128;
    DrawLine(x * 16 + 8, y * 16 + 8, xx * 16 + 8, yy * 16 + 8, color);
}

void Simulation::draw_debug() {
    for (int i = 0; i < m_map.chunks().size(); ++i) {
        DrawRectangleLines((i % m_map.m_chunk_x_count) * m_map.m_chunk_width * 16.0f,
                           (i / m_map.m_chunk_y_count) * m_map.m_chunk_height * 16.0f, m_map.m_chunk_width * 16.0f,
                           m_map.m_chunk_height * 16.0f, LIGHTGRAY);

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
