#include "util.h"
#include <CRC.h>
#include <algorithm>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

bool cd_assets() {
    bool assets_found = false;
    auto original_cwd = fs::current_path();
    for (int i = 0; i < 5; ++i) {
        if (fs::exists("assets")) {
            assets_found = true;
            break;
        }

        fs::current_path("..");
    }
    if (!assets_found) {
        fs::current_path(original_cwd);
    }
    return assets_found;
}

std::vector<std::string> files_in_dir(const std::string& path) {
    std::vector<std::string> files{};
    for (const auto& p : fs::directory_iterator(path)) {
        if (p.is_regular_file()) files.push_back(p.path().string());
    }
    return files;
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