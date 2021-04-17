#include "Wolf.h"

#include <map>

#include "../Util.h"

Wolf::Wolf(float energy) : Agent(AgentType::WOLF, WOLF_SENSOR, energy) {
    m_texture = std::make_shared<Texture2D>(LoadTexture("../assets/wolf.png"));
}

Wolf::~Wolf() = default;

void Wolf::draw(int x, int y) {
    draw_frame(m_texture, x, y, 1);
}

std::pair<int, int> Wolf::calculate_move(std::vector<std::shared_ptr<Field>> surroundings, std::shared_ptr<Field> start_field) {
    m_last_update = GetTime();
    --m_energy;

    auto start_x = start_field->get_pos().first;
    auto start_y = start_field->get_pos().second;
    std::map<double, std::vector<std::shared_ptr<HeatField>>> discrete_heat_map;
    for (auto& field : surroundings) {
        if (field == start_field) {
            continue;
        }
        if (!field->empty()) {
            auto agent = field->agent;
            if (agent->get_type() == AgentType::CHICKEN && !agent->want_to_breed()) {
                continue;
            }
        }
        auto x = field->get_pos().first;
        auto y = field->get_pos().second;
        auto heat_field = std::make_shared<HeatField>(x, y);
        heat_field->distance = sqrt(pow(start_x - x, 2) + pow(start_y - y, 2));
        for(auto& compare_field : surroundings){
            if(!compare_field->empty()){
                auto compare_x = compare_field->get_pos().first;
                auto compare_y = compare_field->get_pos().second;
                auto compare_agent = compare_field->agent;
                auto distance = sqrt(pow(x - compare_x, 2) + pow(compare_y - y, 2));
                switch (compare_field->agent->get_type()) {
                    case AgentType::CABBAGE:
                        heat_field->ca_dist = std::min(heat_field->ca_dist, distance);
                        break;
                    case AgentType::CHICKEN:
                        heat_field->ch_dist = std::min(heat_field->ch_dist, distance);
                        break;
                    case AgentType::WOLF:
                        if (compare_agent->want_to_breed()) {
                            heat_field->ch_dist = std::min(heat_field->ch_dist, distance);
                        }
                        break;
                }
            }
        }

        auto metric = calculate_metric(heat_field);
        if(in_range(heat_field->get_pos(), start_field->get_pos(), 1)){
            discrete_heat_map[metric].push_back(heat_field);
        }
    }

    if(!discrete_heat_map.empty()){
        auto heat_vector = discrete_heat_map.begin()->second;
        auto index = GetRandomValue(0, heat_vector.size() - 1);
        auto target = heat_vector[index];
        return target->get_pos();
    }

    return start_field->get_pos();
}

double Wolf::calculate_metric(std::shared_ptr<HeatField> field) const {
    if(want_to_breed()){
        return field->distance + field->ch_dist + field->fo_dist;
    }
    return field->distance + field->ch_dist;
}
