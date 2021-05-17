#include "Chicken.h"

#include <map>

#include "../ResourceManager.h"
#include "../Util.h"

Chicken::Chicken(int energy) : Agent(AgentType::CHICKEN, CHICKEN_SENSOR, energy) {
    m_texture = ResourceManager::the().get_texture(SimulationTexture::Chicken);
}

Chicken::~Chicken() = default;

void Chicken::apply_field_metrics(Field* field, const Agent& compare_agent, int distance) const {
    switch (compare_agent.get_type()) {
        case AgentType::CABBAGE: field->ca_dist = std::min(field->ca_dist, distance); break;
        case AgentType::CHICKEN:
            if (compare_agent.want_to_breed()) {
                field->ch_dist = std::min(field->ch_dist, distance);
            }
            break;
        case AgentType::WOLF: field->fo_dist = std::min(field->fo_dist, distance); break;
    }
}

int Chicken::calculate_metric(const Field* field) const {
    if (want_to_breed()) {
        return field->distance + field->ca_dist + field->ch_dist + (BASE_METRIC_VALUE - field->fo_dist * 20);
    }
    return field->distance + field->ca_dist + (BASE_METRIC_VALUE - field->fo_dist * 20);
}

void Chicken::apply_behaviour(Field map[HEIGHT][WIDTH], std::vector<std::shared_ptr<Agent>>& offsprings, Field& current_field, Field* target) {
    if (!target->is_empty()) {
        if (target->agent->get_type() == AgentType::CABBAGE) {
            eat(*target->agent);
            target->agent = current_field.agent;
            set_field(target);
            current_field.agent.reset();
        } else if (target->agent->get_type() == AgentType::CHICKEN) {
            auto x = m_field->get_pos().first;
            auto y = m_field->get_pos().second;

            for (int f_y = std::max(0, y - 1); f_y <= std::min(HEIGHT - 1, y + 1); ++f_y) {
                for (int f_x = std::max(0, x - 1); f_x <= std::min(WIDTH - 1, x + 1); ++f_x) {
                    auto& field = map[f_y][f_x];
                    if (field.is_walkable()){
                        auto energy = convert_energy(*target->agent);
                        auto offspring = std::make_shared<Chicken>(energy);
                        field.agent = offspring;
                        offspring->set_field(&field);
                        offsprings.push_back(offspring);
                        break;
                    }
                }
            }
//            for (auto& field : surroundings) {
//                if (field_in_range(*field, current_field, 1) && field->is_walkable()) {
//                    auto energy = convert_energy(*target->agent);
//                    auto offspring = std::make_shared<Chicken>(energy);
//                    field->agent = offspring;
//                    offspring->set_field(field);
//                    offsprings.push_back(offspring);
//                    break;
//                }
//            }
        }
    } else {
        target->agent = current_field.agent;
        set_field(target);
        current_field.agent.reset();
    }
}

void Chicken::draw(int x, int y) {
    draw_frame(m_texture, x, y, 1);
}