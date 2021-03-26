#pragma once

#include "AnimatedAgent.h"
#include "../Util.h"

void AnimatedAgent::draw() {
    auto width = static_cast<float>(m_texture->width) / 4;
    auto height = static_cast<float>(m_texture->height) / 4;

    auto rect = Rectangle{static_cast<float>(m_frame_x) * width, static_cast<float>(m_frame_y) * height, width, height};

    DrawTextureRec(*m_texture, rect, convert_to_pos(m_x, m_y), RAYWHITE);
}
