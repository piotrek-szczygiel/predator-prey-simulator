#include "Field.h"

Field::Field(int x, int y) : m_x{x}, m_y{y}, agent{} {}

void Field::draw() const {
    if(!empty()){
        agent->draw(m_x, m_y);
    }
}

std::pair<int, int> Field::get_pos() {
    return {m_x, m_y};
}

bool Field::agent_need_update() const {
    if(empty()){
        return false;
    }

    return agent->need_update();
}

bool Field::empty() const {
    if(agent){
        return false;
    }

    return true;
}

bool Field::operator==(const Field& rhs) const {
    return m_x == rhs.m_x && m_y == rhs.m_y;
}
