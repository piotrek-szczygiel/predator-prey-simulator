#pragma once

#include <raylib.h>
#include <memory>

#include "AnimatedAgent.h"

class Chicken : public AnimatedAgent {
   public:
    explicit Chicken(int x, int y);
    ~Chicken() override;
};
