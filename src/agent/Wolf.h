#pragma once

#include "Agent.h"

constexpr int WOLF_SENSOR {3};

class Wolf : public Agent {
   public:
    explicit Wolf(float energy = BASE_ENERGY_VALUE);
    ~Wolf() override;
    void draw(int x, int y) override;
    void update(std::vector<std::shared_ptr<Field>> surroundings, std::shared_ptr<Field>& start_field) override;

   protected:
    double calculate_metric(std::shared_ptr<Field> field) const override;
};

