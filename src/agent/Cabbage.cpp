#include "Cabbage.h"
#include <iostream>

#include "../ResourceManager.h"
#include "../Util.h"

Cabbage::Cabbage() : Agent(AgentType::CABBAGE, CABBAGE_SENSOR, BASE_ENERGY_VALUE) {
    m_texture = ResourceManager::the().get_texture(SimulationTexture::Cabbage);
}

Cabbage::~Cabbage() = default;

void Cabbage::update(Field map[HEIGHT][WIDTH], std::vector<std::shared_ptr<Agent>>& offsprings) {
    m_last_update = GetTime();

    auto& current_field = *m_field;

    if (!is_alive()) {
        if (!eaten) {
            current_field.agent.reset();
        }
        return;
    }

    auto x = m_field->get_pos().first;
    auto y = m_field->get_pos().second;

    std::vector<Field*> empty_fields;
    for (int f_y = std::max(0, y - 1); f_y <= std::min(HEIGHT - 1, y + 1); ++f_y) {
        for (int f_x = std::max(0, x - 1); f_x <= std::min(WIDTH - 1, x + 1); ++f_x) {
            auto field = &map[f_y][f_x];
            if (field->is_empty()) {
                empty_fields.push_back(field);
            }
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

void Cabbage::apply_field_metrics(Field* field, const Agent& compare_agent, int distance) const {}

int Cabbage::calculate_metric(const Field* field) const {
    return 0.0f;
}

void Cabbage::apply_behaviour(Field map[HEIGHT][WIDTH], std::vector<std::shared_ptr<Agent>>& offsprings, Field& current_field,
                              Field* target) {}

void Cabbage::draw(int x, int y) {
    DrawTextureV(m_texture, convert_to_pos(x, y), RAYWHITE);
}