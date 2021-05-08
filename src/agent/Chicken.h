#pragma once

#include "Agent.h"

class Chicken : public Agent {
   public:
    explicit Chicken(float energy = BASE_ENERGY_VALUE);
    ~Chicken() override;
    void draw(int x, int y) override;

   protected:
    void apply_field_metrics(Field *field, const Agent &compare_agent, double distance) const override;
    double calculate_metric(const Field *field) const override;
    void apply_behaviour(const std::vector<Field *> &surroundings, std::vector<std::shared_ptr<Agent>> &offsprings, Field &current_field,
                         Field *target) override;
};
