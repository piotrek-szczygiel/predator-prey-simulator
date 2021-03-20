#pragma once

#include <raylib.h>
#include <memory>
#include <array>

#include "../agent/AbstractAgent.h"
#include "../agent/Cabbage.h"

constexpr int START_CABBAGES{10};
constexpr int WIDTH {20};
constexpr int HEIGHT {20};

class shared_ptr;
class Map {
   private:
    std::array<std::array<std::shared_ptr<AbstractAgent>, WIDTH>, HEIGHT> agents {};

    std::shared_ptr<Texture2D> m_background;

   public:
    Map();

    void draw();
};
