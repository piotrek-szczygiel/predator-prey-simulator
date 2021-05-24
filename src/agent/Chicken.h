#pragma once

#include "Agent.h"

class Chicken : public Agent {
   public:
    explicit Chicken(int energy = BASE_ENERGY_VALUE);
    ~Chicken() override;
    void draw(int x, int y) override;

   protected:
    void apply_field_metrics(Field *field, const Agent &compare_agent, int distance) const override;
    int calculate_metric(const Field *field) const override;
    void apply_behaviour(Field map[HEIGHT][WIDTH], std::vector<std::shared_ptr<Agent>> &offsprings, Field &current_field,
                         Field *target) override;
};
