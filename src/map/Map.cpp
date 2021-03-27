#include "Map.h"
#include "../agent/Chicken.h"
#include "../agent/Wolf.h"
#include "../Util.h"

Map::Map() {
    m_background = std::make_shared<Texture2D>(LoadTexture("../assets/ground.png"));

    for (int i = 0; i < START_CABBAGES; ++i) {
        auto x = GetRandomValue(0, WIDTH - 1);
        auto y = GetRandomValue(0, HEIGHT - 1);

        cabbages.push_back(std::make_shared<Cabbage>(x, y));
    }

    agents.push_back(std::make_shared<Chicken>(7, 18));
    agents.push_back(std::make_shared<Wolf>(8, 18));
}

void Map::draw() {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            DrawTextureV(*m_background, convert_to_pos(x, y), RAYWHITE);
        }
    }

    for (const auto& cabbage: cabbages) {
        cabbage->draw();
    }

    for (const auto& agent : agents) {
        agent->draw();
    }
}
