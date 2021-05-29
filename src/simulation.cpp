#include "simulation.h"
#include "simulation_values.h"
#include "util.h"

void Simulation::reset() {
    m_agents.clear();
    m_map.clear();
    m_map.assign((size_t)m_width * m_height, nullptr);

    spawn_random_agents(AgentType::Chicken, CHICKEN_START_COUNT);
    spawn_random_agents(AgentType::Wolf, WOLF_START_COUNT);
    spawn_random_agents(AgentType::Cabbage, CABBAGE_SPAWN_COUNT);
}

void Simulation::update() {
    if (m_tick - m_last_cabbages_spawn > CABBAGE_SPAWN_TIME) {
        spawn_random_agents(AgentType::Cabbage, CABBAGE_SPAWN_COUNT);
        m_last_cabbages_spawn = m_tick;
    }

    for (auto& chunk : m_agents.chunks) {
        for (auto it = chunk.begin(); it != chunk.end();) {
            it->energy -= ENERGY_TICK_LOSS;

            if (it->is_dead()) {
                at(it->x, it->y) = nullptr;
                it = chunk.erase(it);
                continue;
            }

            switch (it->type) {
                case AgentType::Chicken: update_chicken(&*it); break;
                default: break;
            }

            ++it;
        }
    }

    ++m_tick;
}

AgentType Simulation::type_at(int x, int y) const {
    Agent* agent = m_map.at(id(x, y));
    return agent ? agent->type : AgentType::None;
}

int Simulation::count(AgentType type) const {
    return m_agents.count(type);
}

void Simulation::add_agent(int x, int y, AgentType type) {
    Agent* agent = m_agents.add(x, y, type);
    at(x, y) = agent;
}

void Simulation::move_agent(Agent* agent, int x, int y) {
    at(x, y) = agent;
    at(agent->x, agent->y) = nullptr;
    agent->x = x;
    agent->y = y;
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
