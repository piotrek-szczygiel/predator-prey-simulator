#pragma once
#include "config.h"
#include "util.h"

enum class AgentType { Wolf, Chicken, Grass };

struct AgentGenes {
    int offsprings;
    int sensor_range;
};

class Agent {
   public:
    Agent(AgentType type, AgentGenes genes, Vec2 pos, int energy, Tick last_update)
        : type(type), genes(genes), pos(pos), energy(energy), last_update(last_update) {}

    bool is_dead() const { return energy <= 0; }

    void kill() { energy = 0; }

    const char *type_str() const {
        switch (type) {
            case AgentType::Wolf: return "wolf";
            case AgentType::Chicken: return "chicken";
            case AgentType::Grass: return "grass";
            default: return "";
        }
    }

    AgentType type;
    AgentGenes genes;

    Vec2 pos;
    int energy;
    bool hungry = false;

    Tick last_update;
    Vec2 random_direction{};
};