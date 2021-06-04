#include "util.h"
#include <CRC.h>
#include <algorithm>
#include <filesystem>

bool cd_assets() {
    bool assets_found = false;
    auto original_cwd = std::filesystem::current_path();
    for (int i = 0; i < 5; ++i) {
        if (std::filesystem::exists("assets")) {
            assets_found = true;
            break;
        }

        std::filesystem::current_path("..");
    }
    if (!assets_found) {
        std::filesystem::current_path(original_cwd);
    }
    return assets_found;
}

int distance(Vec2 p, Vec2 v) {
    return (p.x - v.x) * (p.x - v.x) + (p.y - v.y) * (p.y - v.y);
}

uint32_t seed_from_str(const std::string& str) {
    if (str.empty()) {
        return 0;
    }

    if (std::find_if(str.begin(), str.end(), [](unsigned char c) { return !std::isdigit(c); }) == str.end()) {
        return std::stoul(str);
    }

    return CRC::Calculate(str.c_str(), str.size(), CRC::CRC_32());
}