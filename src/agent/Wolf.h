#pragma once

#include "Agent.h"

constexpr int WOLF_SENSOR{2};

class Wolf : public Agent {
   public:
    explicit Wolf(float energy = BASE_ENERGY_VALUE);
    ~Wolf() override;
    void draw(int x, int y) override;
    void update(std::vector<Field*>& surroundings, std::vector<std::shared_ptr<Agent>>& offsprings) override;

   protected:
    double calculate_metric(const Field* field) const override;
};
