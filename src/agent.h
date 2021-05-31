#pragma once
#include "config.h"
#include "util.h"

enum class AgentType { None, Wolf, Chicken, Cabbage };

class Agent {
   public:
    Agent(AgentType type, int x, int y, int energy) : type(type), x(x), y(y), energy(energy) {}

    bool is_none() const { return type == AgentType::None; }
    bool is_dead() const { return energy <= 0; }
    void kill() { energy = 0; }

    AgentType type;
    int x;
    int y;
    int energy;
    bool hungry = false;
};