#include "Cabbage.h"
#include <iostream>

#include "../ResourceManager.h"
#include "../Util.h"

Cabbage::Cabbage() : Agent(AgentType::CABBAGE, CABBAGE_SENSOR, BASE_ENERGY_VALUE) {
    m_texture = ResourceManager::the().get_texture(SimulationTexture::Cabbage);
}

Cabbage::~Cabbage() = default;

bool Cabbage::need_update() {
    return GetTime() - m_last_update >= CABBAGE_UPDATE_TIME;
}

void Cabbage::update(std::vector<Field*>& surroundings, std::vector<std::shared_ptr<Agent>>& offsprings) {
    m_last_update = GetTime();

    auto& current_field = *m_field;

    if (!is_alive()) {
        if(!eaten){
            current_field.agent.reset();
        }
        return;
    }

    std::vector<Field*> empty_fields;
    for (auto& field : surroundings) {
        if (field->is_empty()) {
            empty_fields.push_back(field);
        }
    }

    if (!empty_fields.empty()) {
        auto index = GetRandomValue(0, empty_fields.size() - 1);
        auto target = empty_fields[index];
        auto offspring = std::make_shared<Cabbage>();
        offspring->set_field(target);
        target->agent = offspring;
        offsprings.push_back(target->agent);
    }
}

void Cabbage::apply_field_metrics(Field* field, const Agent& compare_agent, double distance) const {}

double Cabbage::calculate_metric(const Field* field) const {
    return 0.0f;
}

void Cabbage::apply_behaviour(const std::vector<Field*>& surroundings, std::vector<std::shared_ptr<Agent>>& offsprings,
                              Field& current_field, Field* target) {
}


void Cabbage::draw(int x, int y) {
    DrawTextureV(m_texture, convert_to_pos(x, y), RAYWHITE);
}