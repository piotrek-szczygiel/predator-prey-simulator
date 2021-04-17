#include "Agent.h"

void Agent::eat(std::shared_ptr<Agent> &prey) {
    m_energy += prey->get_energy();
    if (m_energy > MAX_ENERGY_VALUE) {
        m_energy = MAX_ENERGY_VALUE;
    }
    prey->set_energy(0.0f);
}

bool Agent::need_update() {
    return GetTime() - m_last_update >= DYNAMIC_AGENT_UPDATE_TIME;
}