#include "Cabbage.h"

#include "../Util.h"

Cabbage::Cabbage(int x, int y) : AbstractAgent(x, y) {
    m_texture = std::make_shared<Texture2D>(LoadTexture("../assets/cabbage.png"));
}

void Cabbage::draw() {
    DrawTextureV(*m_texture, convert_to_pos(m_x, m_y), RAYWHITE);
}

Cabbage::~Cabbage() = default;