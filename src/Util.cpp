#include "Util.h"

Vector2 convert_to_pos(int x, int y) {
    auto c_x = static_cast<float>(x * TILE_SIZE);
    auto c_y = static_cast<float>(y * TILE_SIZE);

    return {c_x, c_y};
}
