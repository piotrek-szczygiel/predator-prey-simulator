#pragma once

#include <raylib.h>
#include <array>
#include <memory>
#include <vector>

#include "../agent/Cabbage.h"
#include "../agent/Chicken.h"
#include "../agent/Wolf.h"
#include "../map/Field.h"
#include "SimulationSettings.h"

class Simulation {
   public:
    Simulation();
    virtual ~Simulation();
    void update();
    void draw();
    std::size_t get_agent_count(AgentType agent_type);

   protected:
    Texture2D grass_texture;
    Field m_grid[HEIGHT][WIDTH]{};
    std::vector<std::shared_ptr<Agent>> m_agents{};
    double m_last_cabbages_spawn;

    void spawn_random_cabbages();
};