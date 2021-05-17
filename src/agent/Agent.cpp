#include "Agent.h"

void Agent::eat(Agent& prey) {
    m_energy += prey.get_energy();
    if (m_energy > MAX_ENERGY_VALUE) {
        m_energy = MAX_ENERGY_VALUE;
    }
    prey.set_energy(0.0f);
    prey.eaten = true;
}

bool Agent::need_update() {
    return GetTime() - m_last_update >= DYNAMIC_AGENT_UPDATE_TIME;
}

float Agent::convert_energy(Agent& partner) {
    auto energy = (m_energy + partner.get_energy()) / 4;
    m_energy *= 0.5;
    partner.reduce_energy(0.5f);
    return energy;
}

void Agent::move_to_field(Field& current_field, Field* target) {
    target->agent = current_field.agent;
    set_field(target);
    current_field.agent.reset();
}

void Agent::update(Field map[HEIGHT][WIDTH], std::vector<std::shared_ptr<Agent>>& offsprings) {
    m_last_update = GetTime();
    --m_energy;

    auto& current_field = *m_field;

    if (!is_alive()) {
        if (!eaten) {
            current_field.agent.reset();
        }
        return;
    }

    auto x = m_field->get_pos().first;
    auto y = m_field->get_pos().second;

    std::map<double, std::vector<Field*>> discrete_heat_map;
    for (int f_y = std::max(0, y - m_sensor); f_y <= std::min(HEIGHT - 1, y + m_sensor); ++f_y) {
        for (int f_x = std::max(0, x - m_sensor); f_x <= std::min(WIDTH - 1, x + m_sensor); ++f_x) {
            if (x != f_x || y != f_y) {
                auto& field = map[f_y][f_x];
                if (field == current_field) {
                    continue;
                }
                if (!field.is_empty()) {
                    auto agent = field.agent;
                    if (agent->get_type() == m_type && (!want_to_breed() || !agent->want_to_breed())) {
                        continue;
                    }
                }
                field.reset_metrics();
                field.distance = field.distance_to(current_field);
                for (int s_f_y = std::max(0, y - m_sensor); s_f_y <= std::min(HEIGHT - 1, y + m_sensor); ++s_f_y) {
                    for (int s_f_x = std::max(0, x - m_sensor); s_f_x <= std::min(WIDTH - 1, x + m_sensor); ++s_f_x) {
                        auto& compare_field = map[s_f_y][s_f_x];
                        if (!compare_field.is_empty()) {
                            auto compare_agent = compare_field.agent;
                            auto distance = field.distance_to(compare_field);
                            apply_field_metrics(&field, *compare_agent, distance);
                        }
                    }
                }
                auto metric = calculate_metric(&field);
                if (field_in_range(field, current_field, 1)) {
                    discrete_heat_map[metric].push_back(&field);
                }
            }
        }
    }

//    for (auto& field : surroundings) {
//        if (*field == current_field) {
//            continue;
//        }
//        if (!field->is_empty()) {
//            auto agent = field->agent;
//            if (agent->get_type() == m_type && (!want_to_breed() || !agent->want_to_breed())) {
//                continue;
//            }
//        }
//        field->reset_metrics();
//        field->distance = field->distance_to(current_field);
//        for (auto& compare_field : surroundings) {
//            if (!compare_field->is_empty()) {
//                auto compare_agent = compare_field->agent;
//                auto distance = field->distance_to(*compare_field);
//                apply_field_metrics(field, *compare_agent, distance);
//            }
//        }
//
//        auto metric = calculate_metric(field);
//        if (field_in_range(*field, current_field, 1)) {
//            discrete_heat_map[metric].push_back(field);
//        }
//    }

    if (!discrete_heat_map.empty()) {
        auto heat_vector = discrete_heat_map.begin()->second;
        auto index = GetRandomValue(0, heat_vector.size() - 1);
        auto target = heat_vector[index];

        apply_behaviour(map, offsprings, current_field, target);
    }
}

bool Agent::field_in_range(const Field& target, const Field& start, int range) {
    auto left_part = std::abs(target.get_pos().first - start.get_pos().first) <= range;
    auto right_part = std::abs(target.get_pos().second - start.get_pos().second) <= range;
    return left_part && right_part;
}