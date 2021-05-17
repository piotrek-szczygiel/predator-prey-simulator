#include "Simulation.h"
#include <random>
#include <utility>

#include "../ResourceManager.h"
#include "../Util.h"

Simulation::Simulation() : m_last_cabbages_spawn{GetTime()} {
    grass_texture = ResourceManager::the().get_texture(SimulationTexture::Ground);

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            m_grid[y][x].set_pos(x, y);
        }
    }

    spawn_random_cabbages();

    for (int i = 0; i < START_CHICKENS; ++i) {
        auto x = GetRandomValue(0, WIDTH - 1);
        auto y = GetRandomValue(0, HEIGHT - 1);

        auto chicken = std::make_shared<Chicken>();
        chicken->set_field(&m_grid[y][x]);
        m_grid[y][x].agent = chicken;
        m_agents.push_back(chicken);
    }

    for (int i = 0; i < START_WOLVES; ++i) {
        auto x = GetRandomValue(0, WIDTH - 1);
        auto y = GetRandomValue(0, HEIGHT - 1);

        auto wolf = std::make_shared<Wolf>();
        wolf->set_field(&m_grid[y][x]);
        m_grid[y][x].agent = wolf;
        m_agents.push_back(wolf);
    }
}

Simulation::~Simulation() = default;

void Simulation::update() {
    if (GetTime() - m_last_cabbages_spawn >= CABBAGE_SPAWN_TIME) {
        spawn_random_cabbages();
        m_last_cabbages_spawn = GetTime();
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_agents.begin(), m_agents.end(), g);

    std::vector<std::shared_ptr<Agent>> offsprings;
    for (auto& agent : m_agents) {
        if (agent->need_update()) {
            agent->update(m_grid, offsprings);
        }
    }
    m_agents.insert(m_agents.end(), offsprings.begin(), offsprings.end());
    m_agents.erase(std::remove_if(m_agents.begin(), m_agents.end(), [](const auto& a) { return a.use_count() <= 1; }), m_agents.end());
}

void Simulation::draw() {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            DrawTextureV(grass_texture, convert_to_pos(x, y), RAYWHITE);
        }
    }

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            const auto& field = m_grid[y][x];
            field.draw();
        }
    }
}

void Simulation::spawn_random_cabbages() {
    for (int i = 0; i < START_CABBAGES; ++i) {
        auto x = GetRandomValue(0, WIDTH - 1);
        auto y = GetRandomValue(0, HEIGHT - 1);

        auto& field = m_grid[y][x];
        if (field.is_empty()) {
            auto cabbage = std::make_shared<Cabbage>();
            cabbage->set_field(&field);
            field.agent = cabbage;
            m_agents.push_back(cabbage);
        }
    }
}
