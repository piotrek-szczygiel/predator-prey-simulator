#pragma once

#include "Agent.h"

class Wolf : public Agent {
   public:
    explicit Wolf(float energy = BASE_ENERGY_VALUE);
    ~Wolf() override;
    void draw(int x, int y) override;

   protected:
    void apply_field_metrics(Field* field, const Agent& compare_agent, double distance) const override;
    double calculate_metric(const Field* field) const override;
    void apply_behaviour(Field map[HEIGHT][WIDTH], std::vector<std::shared_ptr<Agent>>& offsprings, Field& current_field,
                         Field* target) override;
};
