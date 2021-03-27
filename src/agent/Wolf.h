#pragma once

#include <raylib.h>
#include <memory>

#include "AbstractAgent.h"

class Wolf : public AbstractAgent {
   public:
    Wolf(int x, int y);
    ~Wolf() override;
};
