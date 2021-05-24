#pragma once

#include <raylib.h>
#include <array>
#include <fstream>
#include <memory>

#include "map/Field.h"

constexpr int TILE_SIZE{16};
constexpr Vector2 MIDDLE{WIDTH * TILE_SIZE / 2.0f, HEIGHT* TILE_SIZE / 2.0f};
constexpr Vector2 CORNER{WIDTH * TILE_SIZE, HEIGHT* TILE_SIZE};

Vector2 convert_to_pos(int x, int y);

void draw_frame(const Texture2D& texture, int x, int y, int frame_x = 0, int frame_y = 0);
void update_camera(Camera2D& camera);

std::ofstream get_csv_stream();