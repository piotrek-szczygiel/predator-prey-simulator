#pragma once

#include <raylib.h>
#include <raymath.h>
#include <memory>

#include "AbstractAgent.h"
class Cabbage : public AbstractAgent {
   public:
    explicit Cabbage(int x, int y);
    ~Cabbage() override;
    void draw() override;
};
