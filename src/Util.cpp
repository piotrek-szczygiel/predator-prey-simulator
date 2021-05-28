#include "util.h"
#include <raylib.h>
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

int random(int min, int max) {
    return GetRandomValue(min, max);
}