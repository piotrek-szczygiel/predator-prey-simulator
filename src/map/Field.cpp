#include "Field.h"

Field::Field() : m_x{}, m_y{}, agent{} {}

void Field::set_pos(int x, int y) {
    m_x = x;
    m_y = y;
}

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

int Field::distance_to(Field& field) const {
    return (m_x - field.m_x) * (m_x - field.m_x) + (m_y - field.m_y) * (m_y - field.m_y);
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
