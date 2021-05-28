#include "simulation.h"
#include "simulation_values.h"
#include "util.h"

void Simulation::reset() {
    m_map.clear();
    m_map.resize((std::size_t)m_width * m_height);
}

void Simulation::update(float time) {
    spawn_cabbages(time);
}

void Simulation::spawn_cabbages(float time) {
    if (time - m_last_cabbages_spawn < CABBAGE_SPAWN_TIME) return;
    m_last_cabbages_spawn = time;
    for (int i = 0; i < CABBAGE_SPAWN_COUNT; ++i) {
        int x = random(0, m_width - 1);
        int y = random(0, m_height - 1);

        if (at(x, y).none()) {
            at(x, y) = Agent(AgentType::Cabbage);
        }
    }
}