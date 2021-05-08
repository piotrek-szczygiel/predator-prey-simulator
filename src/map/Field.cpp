#include "Field.h"

Field::Field(int x, int y) : m_x{x}, m_y{y}, agent{} {}

void Field::draw() const {
    if (!is_empty()) {
        agent->draw(m_x, m_y);
    }
}

void Field::reset_metrics() {
    distance = BASE_METRIC_VALUE;
    ca_dist = BASE_METRIC_VALUE;
    ch_dist = BASE_METRIC_VALUE;
    fo_dist = BASE_METRIC_VALUE;
}

std::pair<int, int> Field::get_pos() const {
    return {m_x, m_y};
}

double Field::distance_to(Field& field) const {
    return sqrt(pow(m_x - field.m_x, 2) + pow(m_y - field.m_y, 2));
}

bool Field::is_empty() const {
    return !agent;
}

bool Field::is_walkable() const {
    return (!is_empty() && agent->get_type() == AgentType::CABBAGE) || is_empty();
}

bool Field::operator==(const Field& rhs) const {
    return m_x == rhs.m_x && m_y == rhs.m_y;
}
