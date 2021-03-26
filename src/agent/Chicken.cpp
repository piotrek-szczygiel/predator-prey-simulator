#include "Chicken.h"

Chicken::Chicken(int x, int y) : AnimatedAgent(x, y) {
    m_texture = std::make_shared<Texture2D>(LoadTexture("../assets/chicken.png"));
}

Chicken::~Chicken() = default;
