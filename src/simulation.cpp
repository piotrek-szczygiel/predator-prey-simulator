#include "simulation.h"
#include "pathfinder.h"

#ifndef NDEBUG
#include <raylib.h>

void Simulation::draw_debug() {
    for (int i = 0; i < m_map.chunks().size(); ++i) {
        DrawRectangleLines((i % m_map.m_chunk_x_count) * m_map.m_chunk_width * 16,
                           (i / m_map.m_chunk_y_count) * m_map.m_chunk_height * 16, m_map.m_chunk_width * 16,
                           m_map.m_chunk_height * 16, LIGHTGRAY);
    }
    for (const auto& line : m_debug_lines) {
        if (line.from->is_dead() || line.to->is_dead()) continue;
        Color c;
        if (line.from->type == AgentType::Chicken && line.to->type == AgentType::Cabbage)
            c = ORANGE;
        else if (line.from->type == AgentType::Chicken && line.to->type == AgentType::Wolf)
            c = BLACK;
        else if (line.from->type == AgentType::Wolf && line.to->type == AgentType::Chicken)
            c = RED;
        else if (line.from->type == line.to->type)
            c = VIOLET;
        else
            c = WHITE;
        c.a = 128;
        Vector2 from = {(float)line.from->x * 16 + 8, (float)line.from->y * 16 + 8};
        Vector2 to = {(float)line.to->x * 16 + 8, (float)line.to->y * 16 + 8};
        DrawLineEx(from, to, 2.0f, c);
    }
}
#endif

void Simulation::reset() {
    m_map.clear();
    m_grid.clear();
    m_grid.resize(m_config.sim_height, std::vector<Agent*>(m_config.sim_width, nullptr));

    spawn_random_agents(AgentType::Chicken, m_config.chicken_spawn_count);
    spawn_random_agents(AgentType::Wolf, m_config.wolf_spawn_count);
    spawn_random_agents(AgentType::Cabbage, m_config.cabbage_spawn_count);
}

void Simulation::update() {
#ifndef NDEBUG
    m_debug_lines.clear();
#endif

    if (m_tick - m_last_cabbages_spawn > m_config.cabbage_spawn_time) {
        spawn_random_agents(AgentType::Cabbage, m_config.cabbage_spawn_count);
        m_last_cabbages_spawn = m_tick;
    }

    for (auto& chunk : m_map.chunks()) {
        for (auto& agent : chunk) {
            agent.energy -= m_config.sim_energy_tick_loss;
            if (agent.is_dead()) {
                if (&agent == m_grid[agent.y][agent.x]) m_grid[agent.y][agent.x] = nullptr;
                continue;
            }

            switch (agent.type) {
                case AgentType::Chicken: update_chicken(&agent); break;
                case AgentType::Wolf: update_wolf(&agent); break;
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

int Simulation::count(AgentType type) const {
    return m_map.count(type);
}

void Simulation::add_agent(AgentType type, int x, int y) {
    Agent* agent = m_map.add(type, x, y, m_config.sim_energy_start);
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
        int x = random(0, m_config.sim_width - 1);
        int y = random(0, m_config.sim_height - 1);

        if (!m_grid[y][x]) {
            add_agent(type, x, y);
        }
    }
}

Vec2 Simulation::get_step_to(Agent* from, AgentType to, int sensor_range) {
    Pathfinder pathfinder(sensor_range, m_config.sim_width, m_config.sim_height, from->x, from->y);

    int min_distance = INT32_MAX;
    Agent* min_agent = nullptr;

    for (const auto& other : m_map.get_nearby_to(from)) {
        int dist = distance({from->x, from->y}, {other->x, other->y});
        if (dist <= sensor_range) {
            pathfinder.add_blocker({other->x, other->y});
            if (other->type == to && dist < min_distance) {
                min_distance = dist;
                min_agent = other;
            }
        }
    }

    if (min_agent) {
#ifndef NDEBUG
        m_debug_lines.push_back({from, min_agent});
#endif
        return pathfinder.get_next_step({min_agent->x, min_agent->y});
    }

    return {0, 0};
}

void Simulation::update_chicken(Agent* chicken) {
    int xx = chicken->x;
    int yy = chicken->y;

    Vec2 step = get_step_to(chicken, AgentType::Wolf, m_config.chicken_sensor_range);
    if (step.x != 0 && step.y != 0) {
        step.x *= -1;
        step.y *= -1;
    } else if (chicken->energy <= m_config.chicken_hungry_start) {
        step = get_step_to(chicken, AgentType::Cabbage, m_config.chicken_sensor_range);
    } else if (chicken->energy >= m_config.sim_energy_breed_needed) {
        step = get_step_to(chicken, AgentType::Chicken, m_config.chicken_sensor_range);
    }

    xx += step.x;
    yy += step.y;

    if (out_of_map(xx, yy) || (step.x == 0 && step.y == 0)) {
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
        chicken->energy += m_config.cabbage_nutrition_value;
        move_agent(chicken, xx, yy);
    } else if (dest->type == AgentType::Chicken && chicken->energy >= m_config.sim_energy_breed_needed) {
        spawn_random_agents(AgentType::Chicken, 1);
        chicken->energy -= m_config.sim_energy_breed_loss;
        dest->energy -= m_config.sim_energy_breed_loss;
    }
}

void Simulation::update_wolf(Agent* wolf) {
    Vec2 step = get_step_to(wolf, AgentType::Chicken, m_config.wolf_sensor_range);
    if (step.x != 0 || step.y != 0) {
        int xx = wolf->x + step.x;
        int yy = wolf->y + step.y;

        Agent* dest = m_grid[yy][xx];
        if (!dest) {
            move_agent(wolf, xx, yy);
        } else if (dest->type == AgentType::Chicken) {
            dest->kill();
            wolf->energy += m_config.chicken_nutrition_value;
            move_agent(wolf, xx, yy);
        }
    }
}