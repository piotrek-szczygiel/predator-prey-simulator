#include "Wolf.h"

Wolf::Wolf(int x, int y) : AnimatedAgent(x, y) {
    m_texture = std::make_shared<Texture2D>(LoadTexture("../assets/wolf.png"));
}

Wolf::~Wolf() = default;
