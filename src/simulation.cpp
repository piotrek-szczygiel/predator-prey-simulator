#include "simulation.h"
#include <numeric>

void Simulation::reset() {
    m_map.clear();
    m_grid.clear();

    spawn_random_agents(AgentType::Wolf, {1, m_config.wolf_sensor_range}, m_config.wolf_spawn_count);
    spawn_random_agents(AgentType::Chicken, {1, m_config.chicken_sensor_range}, m_config.chicken_spawn_count);
    spawn_random_agents(AgentType::Grass, {}, m_config.grass_spawn_count);
}

void Simulation::update() {
    m_debug_lines.clear();
    m_debug_breeds.clear();

    auto start = std::chrono::high_resolution_clock::now();

    m_map.update_chunks();
    m_map.remove_dead();

    if (m_tick - m_last_grass_spawn > m_config.grass_spawn_time) {
        spawn_random_agents(AgentType::Grass, {}, m_config.grass_spawn_count);
        m_last_grass_spawn = m_tick;
    }

    for (auto& chunk : m_map.chunks()) {
        for (auto& agent : chunk) {
            agent.energy -= m_config.sim_energy_tick_loss + agent.genes.sensor_range;
            if (agent.energy <= 0) {
                if (&agent == m_grid.at(agent.pos)) m_grid.at_mut(agent.pos) = nullptr;
                continue;
            }

            switch (agent.type) {
                case AgentType::Chicken: update_chicken(&agent); break;
                case AgentType::Wolf: update_wolf(&agent); break;
                default: break;
            }
        }
    }

    ++m_tick;

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;
    m_update_times.push_back(elapsed.count());

    std::chrono::duration<double, std::milli> average_elapsed = end - m_last_average;
    if (average_elapsed.count() >= 500.0f) {
        m_last_average = end;
        m_avg_update_time =
            std::accumulate(m_update_times.begin(), m_update_times.end(), 0.0) / (double)m_update_times.size();
        m_update_times.clear();
    }
}

int Simulation::count(AgentType type) const {
    return m_map.count(type);
}

AgentGenes Simulation::mutate_genes(AgentGenes mom, AgentGenes dad) {
    int new_offsprings = random(0, 1) ? mom.offsprings : dad.offsprings;
    new_offsprings += random(-1, 1);

    int new_sensor_range = random(0, 1) ? mom.sensor_range : dad.sensor_range;
    new_sensor_range += random(-1, 1);

    return {std::clamp(new_offsprings, 1, m_config.genes_max_sensor_range),
            std::clamp(new_sensor_range, 1, m_config.genes_max_offsprings)};
}

void Simulation::add_agent(AgentType type, AgentGenes genes, Vec2 pos) {
    Agent* agent = m_map.add(type, genes, pos, m_config.sim_energy_start, m_tick - random(0, 2));
    m_grid.at_mut(pos) = agent;
}

void Simulation::move_agent(Agent* agent, Vec2 pos) {
    Vec2 old = agent->pos;
    m_map.move(agent, pos);
    agent->last_update = m_tick;
    m_grid.at_mut(pos) = agent;
    m_grid.at_mut(old) = nullptr;
}

bool Simulation::move_agent_if_empty(Agent* agent, Vec2 pos) {
    if (empty(pos)) {
        move_agent(agent, pos);
        return true;
    } else if (!out_of_map(pos) && m_grid.at(pos) && m_grid.at(pos)->type == AgentType::Grass) {
        m_grid.at(pos)->kill();
        move_agent(agent, pos);
        return true;
    }
    return false;
}

void Simulation::move_agent_random(Agent* agent) {
    std::shuffle(m_possible_random_moves.begin(), m_possible_random_moves.end(), m_mt19937);
    for (const auto& pos : m_possible_random_moves) {
        if (move_agent_if_empty(agent, agent->pos + pos)) break;
    }
}

void Simulation::move_agent_around(Agent* agent, Vec2 pos) {
    if (!move_agent_if_empty(agent, pos)) {
        move_agent_random(agent);
    }
}

void Simulation::spawn_random_agents(AgentType type, AgentGenes genes, int count) {
    for (int i = 0; i < count; ++i) {
        spawn_around(type, genes, random_position());
    }
}

Agent* Simulation::spawn_around(AgentType type, AgentGenes genes, Vec2 pos) {
    std::shuffle(m_possible_spawn_offsets.begin(), m_possible_spawn_offsets.end(), m_mt19937);
    for (const auto& offset : m_possible_spawn_offsets) {
        Vec2 new_pos = pos + offset;
        if (empty(new_pos)) {
            add_agent(type, genes, new_pos);
            return m_grid.at(new_pos);
        }
    }
    return nullptr;
}

void Simulation::breed(Agent* mom, Agent* dad) {
    int offsprings = random(1, mom->genes.offsprings);
    int kid_energy = m_config.sim_energy_breed_loss / offsprings;

    for (int i = 0; i < offsprings; ++i) {
        if (auto kid = spawn_around(mom->type, mutate_genes(mom->genes, dad->genes), mom->pos)) {
            kid->energy = kid_energy * 2;
            mom->energy -= kid_energy;
            dad->energy -= kid_energy;
            if (m_config.runtime_debug_draw) m_debug_breeds.push_back({mom, dad, kid});
        }
    }
}

void Simulation::update_chicken(Agent* chicken) {
    if (chicken->last_update > 0 && m_tick - chicken->last_update < 3) return;

    if (chicken->energy <= m_config.chicken_hunger_start) {
        chicken->hungry = true;
    } else if (chicken->energy >= m_config.chicken_hunger_stop) {
        chicken->hungry = false;
    }

    auto wolf = get_path_to_nearest(chicken, AgentType::Wolf);
    if (wolf.agent) {
        move_agent_around(chicken, chicken->pos - wolf.step);
        return;
    }

    if (chicken->hungry) {
        auto grass = get_path_to_nearest(chicken, AgentType::Grass);
        if (grass.agent) {
            if (grass.dist == 1) {
                grass.agent->kill();
                chicken->energy += m_config.grass_nutrition_value;
            }
            move_agent_around(chicken, chicken->pos + grass.step);
            return;
        }
    }

    if (chicken->energy >= m_config.sim_energy_breed_needed) {
        auto partner = get_path_to_nearest(chicken, AgentType::Chicken, m_config.sim_energy_breed_needed);

        if (partner.agent) {
            if (partner.dist == 1) {
                breed(chicken, partner.agent);
            } else {
                move_agent_around(chicken, chicken->pos + partner.step);
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
        auto chicken = get_path_to_nearest(wolf, AgentType::Chicken);
        if (chicken.agent) {
            wolf->random_direction = {};
            if (chicken.dist == 1) {
                chicken.agent->kill();
                wolf->energy += m_config.chicken_nutrition_value;
            }
            move_agent_around(wolf, wolf->pos + chicken.step);
            return;
        }
    }

    if (wolf->energy >= m_config.sim_energy_breed_needed) {
        auto partner =
            get_path_to_nearest(wolf, AgentType::Wolf, m_config.sim_energy_breed_needed);

        if (partner.agent) {
            wolf->random_direction = {};
            if (partner.dist == 1) {
                breed(wolf, partner.agent);
            } else {
                move_agent_around(wolf, wolf->pos + partner.step);
            }
            return;
        }
    }

    if (wolf->random_direction == Vec2{0, 0}) {
        wolf->random_direction = random_position();
    }

    auto path = m_pathfinder.get_next_step(wolf->pos, wolf->random_direction, m_grid);
    if (distance(wolf->pos, wolf->random_direction) == 1) wolf->random_direction = {};
    move_agent_around(wolf, wolf->pos + path);
}

Path Simulation::get_path_to_nearest(Agent* from, AgentType to, int to_min_energy) {
    int min_distance = INT32_MAX;
    Agent* min_agent = nullptr;

    for (const auto& other : m_map.get_nearby_to(from)) {
        int dist = distance(from->pos, other->pos);
        if (dist <= from->genes.sensor_range * from->genes.sensor_range) {
            if (other->type == to && other->energy >= to_min_energy && dist < min_distance) {
                min_distance = dist;
                min_agent = other;
            }
        }
    }

    if (min_agent) {
        if (m_config.runtime_debug_draw) m_debug_lines.push_back({from, min_agent});
        return {m_pathfinder.get_next_step(from->pos, min_agent->pos, m_grid), min_agent, min_distance};
    }

    return {{0, 0}, nullptr};
}