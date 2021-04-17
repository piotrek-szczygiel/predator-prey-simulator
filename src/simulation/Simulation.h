#pragma once

#include <raylib.h>
#include <array>
#include <memory>
#include <vector>

#include "../agent/Cabbage.h"
#include "../agent/Chicken.h"
#include "../agent/Wolf.h"
#include "../map/Field.h"

constexpr int WIDTH{42};
constexpr int HEIGHT{29};
constexpr int START_CABBAGES{48};
constexpr int START_CHICKENS{24};
constexpr int START_WOLVES{8};
constexpr float CABBAGE_SPAWN_TIME{6.0f};

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
    void move_and_eat(std::shared_ptr<Agent>& agent, std::shared_ptr<Agent>& target_agent);
    void breed(std::shared_ptr<Field>& agent, std::shared_ptr<Field>& target_agent);
    std::shared_ptr<Field> nearest_breeding_field(const std::shared_ptr<Field>& first_field, const std::shared_ptr<Field>& second_field);
    void spawn_random_cabbages();
};