#pragma once

#include "Agent.h"

class Cabbage : public Agent {
   public:
    Cabbage();
    ~Cabbage() override;
    bool need_update() override;
    void update(std::vector<Field *> &surroundings, std::vector<std::shared_ptr<Agent>> &offsprings) override;
    void draw(int x, int y) override;

   protected:
    double calculate_metric(const Field *field) const override;
    void apply_field_metrics(Field *field, const Agent &compare_agent, double distance) const override;
    void apply_behaviour(const std::vector<Field *> &surroundings, std::vector<std::shared_ptr<Agent>> &offsprings, Field &current_field,
                         Field *target) override;
};
