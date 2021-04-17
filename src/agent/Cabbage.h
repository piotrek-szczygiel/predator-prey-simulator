#pragma once

#include "Agent.h"

constexpr int CABBAGE_SENSOR {1};
constexpr float CABBAGE_UPDATE_TIME {0.1f};

class Cabbage : public Agent {
   public:
    Cabbage();
    ~Cabbage() override;
    void draw(int x, int y) override;
    bool need_update() override;
    std::pair<int, int> calculate_move(std::vector<std::shared_ptr<Field>> surroundings, std::shared_ptr<Field> start_field) override;
    double calculate_metric(std::shared_ptr<HeatField> field) const override;
};
