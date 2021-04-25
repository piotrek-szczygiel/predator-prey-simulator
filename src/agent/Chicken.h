#pragma once

#include "Agent.h"

constexpr int CHICKEN_SENSOR{2};

class Chicken : public Agent {
   public:
    explicit Chicken(float energy = BASE_ENERGY_VALUE);
    ~Chicken() override;
    void draw(int x, int y) override;
    void update(std::vector<Field*>& surroundings, Field& start_field) override;
    double calculate_metric(const Field* field) const override;
};
