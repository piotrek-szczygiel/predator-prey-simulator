#pragma once

#include <raylib.h>
#include <memory>

class AbstractAgent {
   protected:
    int m_x;
    int m_y;
    int m_frame_x;
    int m_frame_y;

    std::shared_ptr<Texture2D> m_texture;

   public:
    explicit AbstractAgent(int x, int y) : m_x{x}, m_y{y}, m_texture{}, m_frame_x{1}, m_frame_y{0} {}
    virtual ~AbstractAgent() = 0;
    virtual void draw();
};
