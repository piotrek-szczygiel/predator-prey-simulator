#pragma once

#include "../agent/Agent.h"
#include "../simulation/SimulationSettings.h"

class Agent;

class Field {
   public:
    std::shared_ptr<Agent> agent;

    Field();
    void set_pos(int x, int y);
    void draw() const;
    void reset_metrics();
    std::pair<int, int> get_pos() const;
    int distance_to(Field& field) const;
    bool is_empty() const;
    bool is_walkable() const;
    bool operator==(const Field& rhs) const;

    int distance{BASE_METRIC_VALUE};
    int ca_dist{BASE_METRIC_VALUE};
    int ch_dist{BASE_METRIC_VALUE};
    int fo_dist{BASE_METRIC_VALUE};

   protected:
    int m_x;
    int m_y;
};
