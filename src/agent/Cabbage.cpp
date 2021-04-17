#include "Cabbage.h"

#include "../Util.h"

Cabbage::Cabbage() : Agent(AgentType::CABBAGE, CABBAGE_SENSOR, BASE_ENERGY_VALUE) {
    m_texture = std::make_shared<Texture2D>(LoadTexture("../assets/cabbage.png"));
}

Cabbage::~Cabbage() = default;

void Cabbage::draw(int x, int y) {
    DrawTextureV(*m_texture, convert_to_pos(x, y), RAYWHITE);
}

std::pair<int ,int> Cabbage::calculate_move(std::vector<std::shared_ptr<Field>> surroundings, std::shared_ptr<Field> start_field) {
    m_last_update = GetTime();
    std::vector<std::shared_ptr<Field>> empty_fields;
    for (auto& field : surroundings) {
        if (!field->agent) {
            empty_fields.push_back(field);
        }
    }

    if (!empty_fields.empty()) {
        auto index = GetRandomValue(0, empty_fields.size() - 1);
        auto target = empty_fields[index];
        return target->get_pos();
    }
    return start_field->get_pos();
}

double Cabbage::calculate_metric(std::shared_ptr<HeatField> field) const {
    return 0.0f;
}

bool Cabbage::need_update() {
    return GetTime() - m_last_update >= CABBAGE_UPDATE_TIME;
}
