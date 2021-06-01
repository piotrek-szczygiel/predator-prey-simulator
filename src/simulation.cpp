#include "simulation.h"
#include <numeric>
#include "pathfinder.h"

void Simulation::reset() {
    m_map.clear();
    m_grid.clear();
    m_grid.resize(m_height, std::vector<Agent*>(m_width, nullptr));

    spawn_random_agents(AgentType::Wolf, m_config.wolf_spawn_count);
    spawn_random_agents(AgentType::Chicken, m_config.chicken_spawn_count);
    spawn_random_agents(AgentType::Grass, m_config.grass_spawn_count);
}

void Simulation::update() {
    m_debug_lines.clear();
    m_debug_breeds.clear();

    auto start = std::chrono::high_resolution_clock::now();
    if (m_tick - m_last_grass_spawn > m_config.grass_spawn_time) {
        spawn_random_agents(AgentType::Grass, m_config.grass_spawn_count);
        m_last_grass_spawn = m_tick;
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
    std::chrono::duration<double, std::milli> elapsed = std::chrono::high_resolution_clock::now() - start;

    if (m_update_times.size() == 10) {
        m_avg_update_time = std::accumulate(m_update_times.begin(), m_update_times.end(), 0.0) / 10.0;
        m_update_times.clear();
    }
    m_update_times.push_back(elapsed.count());
}

int Simulation::count(AgentType type) const {
    return m_map.count(type);
}

void Simulation::add_agent(AgentType type, int x, int y) {
    Agent* agent = m_map.add(type, x, y, m_config.sim_energy_start, m_tick - random(0, 2));
    m_grid[y][x] = agent;
}

void Simulation::move_agent(Agent* agent, int x, int y) {
    int old_x = agent->x;
    int old_y = agent->y;
    m_map.move(agent, x, y);
    agent->last_update = m_tick;
    m_grid[y][x] = agent;
    m_grid[old_y][old_x] = nullptr;
}

bool Simulation::move_agent_if_empty(Agent* agent, int x, int y) {
    if (empty(x, y)) {
        move_agent(agent, x, y);
        return true;
    } else if (!out_of_map(x, y) && m_grid[y][x] && m_grid[y][x]->type == AgentType::Grass) {
        m_grid[y][x]->kill();
        move_agent(agent, x, y);
        return true;
    }
    return false;
}

void Simulation::move_agent_random(Agent* agent) {
    std::vector<Vec2> possible{
        {agent->x - 1, agent->y}, {agent->x + 1, agent->y}, {agent->x, agent->y - 1}, {agent->x, agent->y + 1}};

    std::shuffle(possible.begin(), possible.end(), m_mt19937);
    for (const auto& pos : possible) {
        if (move_agent_if_empty(agent, pos.x, pos.y)) break;
    }
}

void Simulation::move_agent_around(Agent* agent, int x, int y) {
    if (!move_agent_if_empty(agent, x, y)) {
        move_agent_random(agent);
    }
}

void Simulation::spawn_random_agents(AgentType type, int count) {
    for (int i = 0; i < count; ++i) {
        spawn_around(type, random_position());
    }
}

Agent* Simulation::spawn_around(AgentType type, Vec2 p) {
    std::vector<Vec2> possible{{p.x - 1, p.y - 1}, {p.x - 1, p.y}, {p.x - 1, p.y + 1},
                               {p.x, p.y - 1},     {p.x, p.y},     {p.x, p.y + 1},
                               {p.x + 1, p.y - 1}, {p.x + 1, p.y}, {p.x + 1, p.y + 1}};
    std::shuffle(possible.begin(), possible.end(), m_mt19937);
    for (const auto& pos : possible) {
        if (empty(pos.x, pos.y)) {
            add_agent(type, pos.x, pos.y);
            return m_grid[pos.y][pos.x];
        }
    }
    return nullptr;
}

void Simulation::update_chicken(Agent* chicken) {
    if (chicken->last_update > 0 && m_tick - chicken->last_update < 3) return;

    if (chicken->energy <= m_config.chicken_hunger_start) {
        chicken->hungry = true;
    } else if (chicken->energy >= m_config.chicken_hunger_stop) {
        chicken->hungry = false;
    }

    auto wolf = get_path_to_nearest(chicken, AgentType::Wolf, m_config.chicken_sensor_range);
    if (wolf.agent) {
        move_agent_around(chicken, chicken->x - wolf.step.x, chicken->y - wolf.step.y);
        return;
    }

    if (chicken->hungry) {
        auto grass = get_path_to_nearest(chicken, AgentType::Grass, m_config.chicken_sensor_range);
        if (grass.agent) {
            if (grass.dist == 1) {
                grass.agent->kill();
                chicken->energy += m_config.grass_nutrition_value;
            }
            move_agent_around(chicken, chicken->x + grass.step.x, chicken->y + grass.step.y);
            return;
        }
    }

    if (chicken->energy >= m_config.sim_energy_breed_needed) {
        auto partner = get_path_to_nearest(chicken, AgentType::Chicken, m_config.chicken_sensor_range,
                                           m_config.sim_energy_breed_needed);

        if (partner.agent) {
            if (partner.dist == 1) {
                if (auto kid = spawn_around(AgentType::Chicken, {chicken->x, chicken->y})) {
                    chicken->energy -= m_config.sim_energy_breed_loss;
                    partner.agent->energy -= m_config.sim_energy_breed_loss;
                    if (m_config.runtime_debug_draw) m_debug_breeds.push_back({chicken, partner.agent, kid});
                }
            } else {
                move_agent_around(chicken, chicken->x + partner.step.x, chicken->y + partner.step.y);
            }
            return;
        }
    }

    move_agent_random(chicken);
}

void Simulation::update_wolf(Agent* wolf) {
    if (wolf->last_update > 0 && m_tick - wolf->last_update < 2) return;

    if (wolf->energy <= m_config.wolf_hunger_start) {
        wolf->hungry = true;
    } else if (wolf->energy >= m_config.wolf_hunger_stop) {
        wolf->hungry = false;
    }

    if (wolf->hungry) {
        auto chicken = get_path_to_nearest(wolf, AgentType::Chicken, m_config.wolf_sensor_range);
        if (chicken.agent) {
            wolf->random_direction = {};
            if (chicken.dist == 1) {
                chicken.agent->kill();
                wolf->energy += m_config.chicken_nutrition_value;
            }
            move_agent_around(wolf, wolf->x + chicken.step.x, wolf->y + chicken.step.y);
            return;
        }
    }

    if (wolf->energy >= m_config.sim_energy_breed_needed) {
        auto partner =
            get_path_to_nearest(wolf, AgentType::Wolf, m_config.wolf_sensor_range, m_config.sim_energy_breed_needed);

        if (partner.agent) {
            wolf->random_direction = {};
            if (partner.dist == 1) {
                if (auto kid = spawn_around(AgentType::Wolf, {wolf->x, wolf->y})) {
                    wolf->energy -= m_config.sim_energy_breed_loss;
                    partner.agent->energy -= m_config.sim_energy_breed_loss;
                    if (m_config.runtime_debug_draw) m_debug_breeds.push_back({wolf, partner.agent, kid});
                }
            } else {
                move_agent_around(wolf, wolf->x + partner.step.x, wolf->y + partner.step.y);
            }
            return;
        }
    }

    if (wolf->random_direction.x == 0 && wolf->random_direction.y == 0) {
        wolf->random_direction = random_position();
    }

    Pathfinder pathfinder(100, m_width, m_height, wolf->x, wolf->y);
    auto path = pathfinder.get_next_step({wolf->random_direction.x, wolf->random_direction.y});
    if (distance({wolf->x, wolf->y}, wolf->random_direction) == 1) wolf->random_direction = {};
    move_agent_around(wolf, wolf->x + path.x, wolf->y + path.y);
}

Path Simulation::get_path_to_nearest(Agent* from, AgentType to, int sensor_range, int to_min_energy) {
    Pathfinder pathfinder(sensor_range, m_width, m_height, from->x, from->y);

    int min_distance = INT32_MAX;
    Agent* min_agent = nullptr;

    for (const auto& other : m_map.get_nearby_to(from)) {
        int dist = distance({from->x, from->y}, {other->x, other->y});
        if (dist <= sensor_range * sensor_range) {
            if (other->type != AgentType::Grass) pathfinder.add_blocker({other->x, other->y});
            if (other->type == to && other->energy >= to_min_energy && dist < min_distance) {
                min_distance = dist;
                min_agent = other;
            }
        }
    }

    if (min_agent) {
        if (m_config.runtime_debug_draw) m_debug_lines.push_back({from, min_agent});
        return {pathfinder.get_next_step({min_agent->x, min_agent->y}), min_agent, min_distance};
    }

    return {{0, 0}, nullptr};
}
