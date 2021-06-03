#pragma once
#include "config.h"
#include "util.h"

enum class AgentType { None, Wolf, Chicken, Grass };

class Agent {
   public:
    Agent(AgentType type, Vec2 pos, int energy, Tick last_update)
        : type(type), pos(pos), energy(energy), last_update(last_update) {}

    bool is_none() const { return type == AgentType::None; }
    bool is_dead() const { return energy <= 0; }

    void kill() { energy = 0; }

    AgentType type;
    Vec2 pos;
    int energy;
    bool hungry = false;

    Tick last_update;
    Vec2 random_direction{};
};