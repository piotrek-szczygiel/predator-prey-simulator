#pragma once

#include "../agent/Agent.h"

constexpr double BASE_METRIC_VALUE{999};

class Agent;

class Field {
   public:
    std::shared_ptr<Agent> agent{};

    Field(int x, int y);
    void draw() const;
    void reset_metrics();
    std::pair<int ,int> get_pos();
    double distance_to(const std::shared_ptr<Field>& field);
    bool empty() const;
    bool agent_need_update() const;
    bool operator==(const Field & rhs) const;

    double distance{BASE_METRIC_VALUE};
    double ca_dist{BASE_METRIC_VALUE};
    double ch_dist{BASE_METRIC_VALUE};
    double fo_dist{BASE_METRIC_VALUE};

   protected:
    int m_x;
    int m_y;
};
