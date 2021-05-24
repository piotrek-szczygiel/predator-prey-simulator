#include "Util.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

Vector2 convert_to_pos(int x, int y) {
    auto c_x = static_cast<float>(x * TILE_SIZE);
    auto c_y = static_cast<float>(y * TILE_SIZE);

    return {c_x, c_y};
}

void draw_frame(const Texture2D& texture, int x, int y, int frame_x, int frame_y) {
    auto width = static_cast<float>(texture.width) / 4;
    auto height = static_cast<float>(texture.height) / 4;

    auto rect = Rectangle{static_cast<float>(frame_x) * width, static_cast<float>(frame_y) * height, width, height};

    DrawTextureRec(texture, rect, convert_to_pos(x, y), RAYWHITE);
}

void update_camera(Camera2D& camera) {
    constexpr float PAN_SPEED{750.0f};
    constexpr float ZOOM_SPEED{2.0f};

    float dt = GetFrameTime();

    if (IsKeyPressed(KEY_B)) {
        printf("%f %f\n", camera.target.x, camera.target.y);
    }

    camera.target.y -= IsKeyDown(KEY_UP) * dt * PAN_SPEED / camera.zoom;
    camera.target.y += IsKeyDown(KEY_DOWN) * dt * PAN_SPEED / camera.zoom;
    camera.target.x -= IsKeyDown(KEY_LEFT) * dt * PAN_SPEED / camera.zoom;
    camera.target.x += IsKeyDown(KEY_RIGHT) * dt * PAN_SPEED / camera.zoom;
    camera.target.x = std::max(0.0f, std::min(CORNER.x, camera.target.x));
    camera.target.y = std::max(0.0f, std::min(CORNER.y, camera.target.y));

    camera.zoom *= 1.0f + IsKeyDown(KEY_W) * dt * ZOOM_SPEED;
    camera.zoom /= 1.0f + IsKeyDown(KEY_S) * dt * ZOOM_SPEED;
    camera.zoom = std::max(0.1f, std::min(4.0f, camera.zoom));

    if (IsKeyPressed(KEY_X)) {
        camera.target = MIDDLE;
        camera.zoom = 1.5f;
    }
}

std::ofstream get_csv_stream() {
    auto time = std::time(nullptr);
    std::stringstream ss;
    ss << "csv/" << std::put_time(std::localtime(&time), "%F_%T") << ".csv";
    auto s = ss.str();
    std::replace(s.begin(), s.end(), ':', '-');
    std::cout << "creating csv file " << s << std::endl;
    std::ofstream csv(s);
    csv << "predator;prey\n";
    return csv;
}