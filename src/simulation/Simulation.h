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

   protected:
    Texture2D grass_texture;
    std::array<std::array<std::shared_ptr<Field>, WIDTH>, HEIGHT> m_grid{};
    std::vector<std::shared_ptr<Agent>> m_agents{};
    double m_last_cabbages_spawn;

    std::vector<Field*> surroundings(const Field& field, int sensor);
    void spawn_random_cabbages();
};