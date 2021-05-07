#pragma once

#include "Agent.h"

constexpr int CABBAGE_SENSOR{1};
constexpr float CABBAGE_UPDATE_TIME{0.1f};

class Cabbage : public Agent {
   public:
    Cabbage();
    ~Cabbage() override;
    void draw(int x, int y) override;
    bool need_update() override;
    void update(std::vector<Field*>& surroundings, Field& start_field, std::vector<std::shared_ptr<Agent>>& offsprings) override;
    double calculate_metric(const Field* field) const override;
};
