#pragma once

#include "AbstractAgent.h"

class AnimatedAgent : public AbstractAgent {
   protected:
    int m_frame_x;
    int m_frame_y;

   public:
    AnimatedAgent(int x, int y) : AbstractAgent(x, y), m_frame_x{1}, m_frame_y{0} {}
    ~AnimatedAgent() override = default;
    void draw() override;
};
