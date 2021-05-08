#include "Wolf.h"

#include <iostream>
#include <map>
#include <memory>

#include "../ResourceManager.h"
#include "../Util.h"

Wolf::Wolf(float energy) : Agent(AgentType::WOLF, WOLF_SENSOR, energy) {
    m_texture = ResourceManager::the().get_texture(SimulationTexture::Wolf);
}

Wolf::~Wolf() = default;

void Wolf::apply_field_metrics(Field* field, const Agent& compare_agent, double distance) const {
    switch (compare_agent.get_type()) {
        case AgentType::CABBAGE: field->ca_dist = std::min(field->ca_dist, distance); break;
        case AgentType::CHICKEN: field->ch_dist = std::min(field->ch_dist, distance); break;
        case AgentType::WOLF:
            if (compare_agent.want_to_breed()) {
                field->ch_dist = std::min(field->ch_dist, distance);
            }
            break;
    }
}

double Wolf::calculate_metric(const Field* field) const {
    if (want_to_breed()) {
        return field->distance + field->ch_dist + field->fo_dist;
    }
    return field->distance + field->ch_dist;
}

void Wolf::apply_behaviour(const std::vector<Field*>& surroundings, std::vector<std::shared_ptr<Agent>>& offsprings, Field& current_field,
                           Field* target) {
    if (!target->is_empty()) {
        if (target->agent->get_type() == AgentType::CABBAGE) {
            move_to_field(current_field, target);
        } else if (target->agent->get_type() == AgentType::CHICKEN) {
            this->eat(*target->agent);
            move_to_field(current_field, target);
        } else if (target->agent->get_type() == AgentType::WOLF) {
            for (auto& field : surroundings) {
                if (field_in_range(*field, current_field, 1) && field->is_walkable()) {
                    auto energy = this->convert_energy(*target->agent);
                    auto offspring = std::make_shared<Wolf>(energy);
                    field->agent = offspring;
                    offspring->set_field(field);
                    offsprings.push_back(offspring);
                    break;
                }
            }
        }
    } else {
        move_to_field(current_field, target);
    }
}

void Wolf::draw(int x, int y) {
    draw_frame(m_texture, x, y, 1);
}