#pragma once

#include <raylib.h>
#include <memory>

class AbstractAgent {
   protected:
    int m_x;
    int m_y;
    std::shared_ptr<Texture2D> m_texture;

   public:
    explicit AbstractAgent(int x, int y) : m_x{x}, m_y{y}, m_texture{} {}
    virtual ~AbstractAgent() = default;
    virtual void draw() = 0;
};
