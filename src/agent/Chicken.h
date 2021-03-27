#pragma once

#include <raylib.h>
#include <memory>

#include "AbstractAgent.h"

class Chicken : public AbstractAgent {
   public:
    explicit Chicken(int x, int y);
    ~Chicken() override;
};
