#include "Cabbage.h"

#include "../ResourceManager.h"
#include "../Util.h"

Cabbage::Cabbage() : Agent(AgentType::CABBAGE, CABBAGE_SENSOR, BASE_ENERGY_VALUE) {
    m_texture = ResourceManager::the().get_texture(SimulationTexture::Cabbage);
}

Cabbage::~Cabbage() = default;

void Cabbage::draw(int x, int y) {
    DrawTextureV(m_texture, convert_to_pos(x, y), RAYWHITE);
}

double Cabbage::calculate_metric(const Field* field) const {
    return 0.0f;
}

bool Cabbage::need_update() {
    return GetTime() - m_last_update >= CABBAGE_UPDATE_TIME;
}

void Cabbage::update(std::vector<Field*>& surroundings, Field& start_field) {
    m_last_update = GetTime();
    std::vector<Field*> empty_fields;
    for (auto& field : surroundings) {
        if (field->is_empty()) {
            empty_fields.push_back(field);
        }
    }

    if (!empty_fields.empty()) {
        auto index = GetRandomValue(0, empty_fields.size() - 1);
        auto target = empty_fields[index];
        target->agent = std::make_shared<Cabbage>();
    }
}