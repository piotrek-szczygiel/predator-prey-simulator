#include "simulation.h"
#include "pathfinder.h"

#ifndef NDEBUG
#include <raylib.h>

void draw_line(int x, int y, int xx, int yy, Color color) {
    color.a = 128;
    Vector2 from = {(float)x * 16 + 8, (float)y * 16 + 8};
    Vector2 to = {(float)xx * 16 + 8, (float)yy * 16 + 8};
    DrawLineEx(from, to, 2.0f, color);
}
#endif

void Simulation::reset() {
    m_map.clear();
    m_grid.fill({});

    spawn_random_agents(AgentType::Chicken, CHICKEN_SPAWN_COUNT);
    spawn_random_agents(AgentType::Wolf, WOLF_SPAWN_COUNT);
    spawn_random_agents(AgentType::Cabbage, CABBAGE_SPAWN_COUNT);
}

void Simulation::update() {
#ifndef NDEBUG
    for (int i = 0; i < m_map.chunks().size(); ++i) {
        DrawRectangleLines((i % m_map.m_chunk_x_count) * m_map.m_chunk_width * 16.0f,
                           (i / m_map.m_chunk_y_count) * m_map.m_chunk_height * 16.0f, m_map.m_chunk_width * 16.0f,
                           m_map.m_chunk_height * 16.0f, LIGHTGRAY);
    }
#endif

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

Vec2 Simulation::get_step_to(Agent* from, AgentType to, int sensor_range) {
    Pathfinder pathfinder(sensor_range, m_width, m_height, from->x, from->y);

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
        auto c = WHITE;
        if (to == AgentType::Cabbage)
            c = GREEN;
        else if (to == AgentType::Chicken)
            c = RED;
        else
            c = BLACK;
        draw_line(from->x, from->y, min_agent->x, min_agent->y, c);
#endif
        return pathfinder.get_next_step({min_agent->x, min_agent->y});
    }

    return {0, 0};
}

void Simulation::update_chicken(Agent* chicken) {
    int xx = chicken->x;
    int yy = chicken->y;

    Vec2 step = get_step_to(chicken, AgentType::Wolf, CHICKEN_SENSOR_RANGE);
    if (step.x != 0 && step.y != 0) {
        step.x *= -1;
        step.y *= -1;
    } else if (chicken->energy <= CHICKEN_HUNGRY) {
        step = get_step_to(chicken, AgentType::Cabbage, CHICKEN_SENSOR_RANGE);
    } else if (chicken->energy >= BREED_NEEDED_ENERGY) {
        step = get_step_to(chicken, AgentType::Chicken, CHICKEN_SENSOR_RANGE);
    }

    if (step.x == 0 && step.y == 0) {
        do {
            xx = chicken->x + random(-1, 1);
            yy = chicken->y + random(-1, 1);
        } while (out_of_map(xx, yy));
    } else {
        xx += step.x;
        yy += step.y;
    }

    Agent* dest = m_grid[yy][xx];
    if (!dest) {
        move_agent(chicken, xx, yy);
    } else if (dest->type == AgentType::Cabbage) {
        dest->kill();
        chicken->energy += CABBAGE_NUTRITION_VALUE;
        move_agent(chicken, xx, yy);
    } else if (dest->type == AgentType::Chicken && chicken->energy >= BREED_NEEDED_ENERGY) {
        spawn_random_agents(AgentType::Chicken, 1);
        chicken->energy -= BREED_LOSS_ENERGY;
        dest->energy -= BREED_LOSS_ENERGY;
    }
}

void Simulation::update_wolf(Agent* wolf) {
    Vec2 step = get_step_to(wolf, AgentType::Chicken, WOLF_SENSOR_RANGE);
    if (step.x != 0 || step.y != 0) {
        int xx = wolf->x + step.x;
        int yy = wolf->y + step.y;

        Agent* dest = m_grid[yy][xx];
        if (!dest) {
            move_agent(wolf, xx, yy);
        } else if (dest->type == AgentType::Chicken) {
            dest->kill();
            wolf->energy += CHICKEN_NUTRITION_VALUE;
            move_agent(wolf, xx, yy);
        }
    }
}