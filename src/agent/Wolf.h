#pragma once

#include <raylib.h>
#include <memory>

#include "AnimatedAgent.h"

class Wolf : public AnimatedAgent {
   public:
    Wolf(int x, int y);
    ~Wolf() override;
};
