#include "Wolf.h"

#include <iostream>
#include <map>
#include <memory>

#include "../ResourceManager.h"
#include "../Util.h"

Wolf::Wolf(int energy) : Agent(AgentType::WOLF, WOLF_SENSOR, energy) {
    m_texture = ResourceManager::the().get_texture(SimulationTexture::Wolf);
}

Wolf::~Wolf() = default;

void Wolf::apply_field_metrics(Field* field, const Agent& compare_agent, int distance) const {
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

int Wolf::calculate_metric(const Field* field) const {
    if (want_to_breed()) {
        return field->distance + field->ch_dist + field->fo_dist;
    }
    return field->distance + field->ch_dist;
}

void Wolf::apply_behaviour(Field map[HEIGHT][WIDTH], std::vector<std::shared_ptr<Agent>>& offsprings, Field& current_field, Field* target) {
    if (!target->is_empty()) {
        if (target->agent->get_type() == AgentType::CABBAGE) {
            move_to_field(current_field, target);
        } else if (target->agent->get_type() == AgentType::CHICKEN) {
            this->eat(*target->agent);
            move_to_field(current_field, target);
        } else if (target->agent->get_type() == AgentType::WOLF) {
            auto x = m_field->get_pos().first;
            auto y = m_field->get_pos().second;

            for (int f_y = std::max(0, y - 1); f_y <= std::min(HEIGHT - 1, y + 1); ++f_y) {
                for (int f_x = std::max(0, x - 1); f_x <= std::min(WIDTH - 1, x + 1); ++f_x) {
                    auto& field = map[f_y][f_x];
                    if (field.is_walkable()) {
                        auto energy = convert_energy(*target->agent);
                        auto offspring = std::make_shared<Wolf>(energy);
                        field.agent = offspring;
                        offspring->set_field(&field);
                        offsprings.push_back(offspring);
                        break;
                    }
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