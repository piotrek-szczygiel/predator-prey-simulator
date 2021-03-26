#include "Map.h"
#include "../agent/Chicken.h"
#include "../agent/Wolf.h"
#include "../Util.h"

Map::Map() {
    m_background = std::make_shared<Texture2D>(LoadTexture("../assets/ground.png"));

    for (int i = 0; i < START_CABBAGES; ++i) {
        auto x = GetRandomValue(0, WIDTH - 1);
        auto y = GetRandomValue(0, HEIGHT - 1);

        agents[y][x] = std::make_shared<Cabbage>(Cabbage(x, y));
    }

    agents[18][7] = std::make_shared<Chicken>(Chicken(7, 18));
    agents[18][8] = std::make_shared<Wolf>(Wolf(8, 18));
}

void Map::draw() {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            DrawTextureV(*m_background, convert_to_pos(x, y), RAYWHITE);
        }
    }

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (agents[y][x]) {
                agents[y][x]->draw();
            }
        }
    }
}
