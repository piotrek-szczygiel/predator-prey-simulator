#include "Util.h"

Vector2 convert_to_pos(int x, int y) {
    auto c_x = static_cast<float>(x * TILE_SIZE);
    auto c_y = static_cast<float>(y * TILE_SIZE);

    return {c_x, c_y};
}

void draw_frame(const std::shared_ptr<Texture2D>& texture, int x, int y, int frame_x, int frame_y) {
    auto width = static_cast<float>(texture->width) / 4;
    auto height = static_cast<float>(texture->height) / 4;

    auto rect = Rectangle{static_cast<float>(frame_x) * width, static_cast<float>(frame_y) * height, width, height};

    DrawTextureRec(*texture, rect, convert_to_pos(x, y), RAYWHITE);
}

bool in_range(const std::pair<int, int>& target, const std::pair<int, int>& start, int range) {
    auto left_part = std::abs(target.first - start.first) <= range;
    auto right_part = std::abs(target.second - start.second) <= range;
    return left_part && right_part;
}