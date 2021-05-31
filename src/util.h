#pragma once
#include <cstdint>

using Tick = int64_t;
const Tick TICK_MIN = INT64_MIN;

struct Vec2 {
    int x;
    int y;
};

bool cd_assets();
int distance(Vec2 p, Vec2 v);