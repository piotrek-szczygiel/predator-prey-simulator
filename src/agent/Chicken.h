#pragma once

#include "Agent.h"

constexpr int CHICKEN_SENSOR {2};

class Chicken : public Agent {
   public:
    explicit Chicken(float energy = BASE_ENERGY_VALUE);
    ~Chicken() override;
    void draw(int x, int y) override;
    std::pair<int, int> calculate_move(std::vector<std::shared_ptr<Field>> surroundings, std::shared_ptr<Field> start_field) override;
    double calculate_metric(std::shared_ptr<HeatField> field) const override;
};
