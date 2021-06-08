#include "simulation.h"
#include <numeric>

void Simulation::reset() {
    m_map.clear();
    m_grid.clear();

    spawn_random_agents(AgentType::Wolf, {1, m_config.wolf_sensor_range}, m_config.wolf_spawn_count);
    spawn_random_agents(AgentType::Chicken, {1, m_config.chicken_sensor_range}, m_config.chicken_spawn_count);
    spawn_random_agents(AgentType::Grass, {}, m_config.grass_spawn_count);
}

void Simulation::update(Scripting& scripting) {
    m_debug_lines.clear();
    m_debug_breeds.clear();

    auto start = std::chrono::high_resolution_clock::now();

    m_map.update_chunks();
    m_map.remove_dead();

    spawn_random_agents(AgentType::Grass, {}, m_config.grass_spawn_count);

    for (auto& chunk : m_map.chunks()) {
        for (auto& agent : chunk) {
            if (agent.type == AgentType::Chicken) {
                scripting.update_chicken(&agent);
            } else if (agent.type == AgentType::Wolf) {
                scripting.update_wolf(&agent);
            }

            if (agent.is_dead() && m_grid.at(agent.pos) == &agent) {
                m_grid.at_mut(agent.pos) = nullptr;
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

std::tuple<double, double> Simulation::avg_genes() const {
    return m_map.avg_genes();
}

AgentGenes Simulation::mutate_genes(AgentGenes mom, AgentGenes dad) {
    int new_offsprings = random(0, 1) ? mom.offsprings : dad.offsprings;
    new_offsprings += random(-1, 1);

    int new_sensor_range = random(0, 1) ? mom.sensor_range : dad.sensor_range;
    new_sensor_range += random(-1, 1);

    return {std::clamp(new_offsprings, 1, m_config.genes_max_offsprings),
            std::clamp(new_sensor_range, 1, m_config.genes_max_sensor_range)};
}

Vec2 Simulation::random_position_around(Vec2 pos, int dist) {
    Vec2 min = {std::max(0, pos.x - dist), std::max(0, pos.y - dist)};
    Vec2 max = {std::min(m_size.x - 1, pos.x + dist), std::min(m_size.y - 1, pos.y + dist)};
    return {random(min.x, max.x), random(min.y, max.y)};
}

void Simulation::add_agent(AgentType type, AgentGenes genes, Vec2 pos) {
    int energy = 1;
    if (type == AgentType::Chicken) {
        energy = m_config.chicken_energy_start;
    } else if (type == AgentType::Wolf) {
        energy = m_config.wolf_energy_start;
    }
    Agent* agent = m_map.add(type, genes, pos, energy, m_tick - random(0, 2));
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

void Simulation::move_agent_random(Agent* agent, int dist) {
    if (agent->random_direction == Vec2{0, 0}) {
        if (dist == -1) {
            agent->random_direction = random_position();
        } else {
            agent->random_direction = random_position_around(agent->pos, dist);
        }
    }

    auto path = m_pathfinder.get_next_step(agent->pos, agent->random_direction, m_grid);
    if (distance(agent->pos, agent->random_direction) == 1) agent->random_direction = {};
    move_agent_around(agent, agent->pos + path);
}

void Simulation::move_agent_around(Agent* agent, Vec2 pos) {
    if (!move_agent_if_empty(agent, pos)) {
        std::shuffle(m_possible_random_moves.begin(), m_possible_random_moves.end(), m_mt19937);
        for (const auto& move : m_possible_random_moves) {
            if (move_agent_if_empty(agent, agent->pos + move)) break;
        }
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
    int breed_cost = 0;
    if (mom->type == AgentType::Chicken) {
        breed_cost = m_config.chicken_breed_cost;
    } else if (mom->type == AgentType::Wolf) {
        breed_cost = m_config.wolf_breed_cost;
    }

    if (mom->energy < breed_cost || dad->energy < breed_cost) return;
    int offsprings = random(1, mom->genes.offsprings);
    int kid_energy = breed_cost / offsprings;

    for (int i = 0; i < offsprings; ++i) {
        if (auto kid = spawn_around(mom->type, mutate_genes(mom->genes, dad->genes), mom->pos)) {
            kid->energy = kid_energy * 2;
            mom->energy -= kid_energy;
            dad->energy -= kid_energy;
            if (m_config.control_debug) m_debug_breeds.push_back({mom, dad, kid});
        }
    }
}

std::tuple<Agent*, int, Vec2> Simulation::get_path_to_nearest(Agent* from, AgentType to, bool fed) {
    int min_distance = INT32_MAX;
    Agent* min_agent = nullptr;

    for (const auto& other : m_map.get_nearby_to(from)) {
        int dist = distance(from->pos, other->pos);
        if (dist <= from->genes.sensor_range * from->genes.sensor_range) {
            if (other->type == to && (!fed || !other->hungry) && dist < min_distance) {
                min_distance = dist;
                min_agent = other;
            }
        }
    }

    if (min_agent) {
        if (m_config.control_debug) m_debug_lines.push_back({from, min_agent});
        return {min_agent, min_distance, m_pathfinder.get_next_step(from->pos, min_agent->pos, m_grid)};
    }

    return {};
}
