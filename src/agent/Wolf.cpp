#include "Wolf.h"

#include <iostream>
#include <map>

#include "../ResourceManager.h"
#include "../Util.h"

Wolf::Wolf(float energy) : Agent(AgentType::WOLF, WOLF_SENSOR, energy) {
    m_texture = ResourceManager::the().get_texture(SimulationTexture::Wolf);
}

Wolf::~Wolf() = default;

void Wolf::draw(int x, int y) {
    draw_frame(m_texture, x, y, 1);
}

double Wolf::calculate_metric(const Field* field) const {
    if (want_to_breed()) {
        return field->distance + field->ch_dist + field->fo_dist;
    }
    return field->distance + field->ch_dist;
}

void Wolf::update(std::vector<Field*>& surroundings, Field& start_field) {
    m_last_update = GetTime();
    --m_energy;

    std::map<double, std::vector<Field*>> discrete_heat_map;
    for (auto& field : surroundings) {
        if (*field == start_field) {
            continue;
        }
        if (!field->is_empty()) {
            auto agent = field->agent;
            if (agent->get_type() == AgentType::WOLF && (!want_to_breed() || !agent->want_to_breed())) {
                continue;
            }
        }
        field->reset_metrics();
        field->distance = field->distance_to(start_field);
        for (auto& compare_field : surroundings) {
            if (!compare_field->is_empty()) {
                auto compare_agent = compare_field->agent;
                auto distance = field->distance_to(*compare_field);
                switch (compare_field->agent->get_type()) {
                    case AgentType::CABBAGE: field->ca_dist = std::min(field->ca_dist, distance); break;
                    case AgentType::CHICKEN: field->ch_dist = std::min(field->ch_dist, distance); break;
                    case AgentType::WOLF:
                        if (compare_agent->want_to_breed()) {
                            field->ch_dist = std::min(field->ch_dist, distance);
                        }
                        break;
                }
            }
        }

        auto metric = calculate_metric(field);
        if (in_range(*field, start_field, 1)) {
            discrete_heat_map[metric].push_back(field);
        }
    }

    if (!discrete_heat_map.empty()) {
        auto heat_vector = discrete_heat_map.begin()->second;
        auto index = GetRandomValue(0, heat_vector.size() - 1);
        auto target = heat_vector[index];

        if (!target->is_empty()) {
            if (target->agent->get_type() == AgentType::CABBAGE) {
                target->agent = std::make_shared<Wolf>(*this);
                start_field.agent.reset();
            } else if (target->agent->get_type() == AgentType::CHICKEN) {
                eat(*target->agent);
                target->agent = std::make_shared<Wolf>(*this);
                start_field.agent.reset();
            } else if (target->agent->get_type() == AgentType::WOLF) {
                for (auto& field : surroundings) {
                    if (in_range(*field, start_field, 1) && field->is_walkable()) {
                        auto energy = convert_energy(*target->agent);
                        field->agent = std::make_shared<Wolf>(energy);
                        break;
                    }
                }
            }
        } else {
            target->agent = std::make_shared<Wolf>(*this);
            start_field.agent.reset();
        }
    }
}