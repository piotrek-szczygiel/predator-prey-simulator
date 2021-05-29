#pragma once
#include "simulation_values.h"

enum class AgentType { None, Wolf, Chicken, Cabbage };

class Agent {
   public:
    Agent(int x, int y, AgentType type) : x(x), y(y), type(type) {}

    bool is_none() { return type == AgentType::None; }
    bool wants_to_breed() { return energy >= BREED_ENERGY; }
    bool is_dead() { return energy <= 0; }

    AgentType type;
    int x;
    int y;
    int energy = BASE_ENERGY;
};