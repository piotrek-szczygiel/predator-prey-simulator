#pragma once

#include <raylib.h>
#include <raymath.h>
#include <memory>

class Cabbage {
   protected:
    int m_x;
    int m_y;

    std::shared_ptr<Texture2D> m_texture;

   public:
    explicit Cabbage(int x, int y);
    ~Cabbage();
    void draw();
};
