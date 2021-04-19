#pragma once

#include <raylib.h>
#include <array>
#include <memory>
#include <vector>

#include "../agent/Cabbage.h"
#include "../agent/Chicken.h"
#include "../agent/Wolf.h"
#include "../map/Field.h"

constexpr int WIDTH{20};
constexpr int HEIGHT{20};
constexpr int START_CABBAGES{8};
constexpr int START_CHICKENS{10};
constexpr int START_WOLVES{8};
constexpr float CABBAGE_SPAWN_TIME{1.0f};

class Simulation {
   public:
    Simulation();
    virtual ~Simulation();
    void update();
    void draw();

   protected:
    std::shared_ptr<Texture2D> grass_texture;
    std::array<std::array<std::shared_ptr<Field>, WIDTH>, HEIGHT> m_grid{};
    double m_last_cabbages_spawn;

    void update_field(std::shared_ptr<Field>& field);
    std::vector<std::shared_ptr<Field>> surroundings(int x, int y, int sensor);
    void spawn_random_cabbages();
};