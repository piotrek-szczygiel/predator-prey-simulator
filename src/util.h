#pragma once
#include <cstdint>

using Tick = int64_t;

struct Vec2 {
    int x;
    int y;

    inline Vec2 operator+(Vec2 other) const { return {other.x + x, other.y + y}; }
    inline Vec2 operator-(Vec2 other) const { return {other.x - x, other.y - y}; }
};

bool cd_assets();
int distance(Vec2 p, Vec2 v);