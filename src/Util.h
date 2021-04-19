#pragma once

#include <raylib.h>
#include <memory>

#include "map/Field.h"

constexpr int TILE_SIZE{16};

Vector2 convert_to_pos(int x, int y);

void draw_frame(const Texture2D& texture, int x, int y, int frame_x = 0, int frame_y = 0);

bool in_range(const std::pair<int, int>& target, const std::pair<int, int>& start, int range);