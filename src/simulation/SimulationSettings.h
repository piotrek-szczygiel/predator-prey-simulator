#pragma once

// Window settings
constexpr int WINDOW_WIDTH{1600};
constexpr int WINDOW_HEIGHT{900};

// Map settings
constexpr int WIDTH{100};
constexpr int HEIGHT{50};
constexpr int START_CABBAGES{15};
constexpr int START_CHICKENS{20};
constexpr int START_WOLVES{20};

// Random cabbage spawn timer
constexpr float CABBAGE_SPAWN_TIME{5.0f};

// Agents settings
constexpr float BASE_ENERGY_VALUE{40.0f};
constexpr float MAX_ENERGY_VALUE{100.0f};

constexpr int CABBAGE_SENSOR{1};
constexpr int CHICKEN_SENSOR{2};
constexpr int WOLF_SENSOR{3};

constexpr float CABBAGE_UPDATE_TIME{0.1f};
constexpr float DYNAMIC_AGENT_UPDATE_TIME{0.1f};

// Field settings
constexpr double BASE_METRIC_VALUE{999};
