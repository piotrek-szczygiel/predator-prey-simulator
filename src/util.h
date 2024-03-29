#pragma once
#include <cstdint>
#include <filesystem>
#include <small_vector.hpp>
#include <vector>

using Tick = int64_t;

template <typename T>
using SmallVector = itlib::small_vector<T, 1024>;

struct Vec2 {
    int x;
    int y;

    inline Vec2 operator+(Vec2 other) const { return {x + other.x, y + other.y}; }
    inline Vec2 operator-(Vec2 other) const { return {x - other.x, y - other.y}; }
    inline bool operator==(Vec2 other) const { return other.x == x && other.y == y; }
    inline bool operator!=(Vec2 other) const { return other.x != x || other.y != y; }
};

bool cd_assets();
std::vector<std::string> files_in_dir(std::filesystem::path path);
int distance(Vec2 p, Vec2 v);
uint32_t seed_from_str(const std::string& str);
