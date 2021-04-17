#pragma once

#include "../agent/Agent.h"

class Agent;

class Field {
   public:
    std::shared_ptr<Agent> agent{};

    Field(int x, int y);
    void draw() const;
    std::pair<int ,int> get_pos();
    bool empty() const;
    bool agent_need_update() const;
    bool operator==(const Field & rhs) const;

   protected:
    int m_x;
    int m_y;
};
