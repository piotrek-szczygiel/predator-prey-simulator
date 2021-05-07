#include "Simulation.h"
#include <utility>
#include <random>

#include "../ResourceManager.h"
#include "../Util.h"

Simulation::Simulation() : m_last_cabbages_spawn{GetTime()} {
    grass_texture = ResourceManager::the().get_texture(SimulationTexture::Ground);

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            m_grid[y][x] = std::make_shared<Field>(x, y);
        }
    }

    spawn_random_cabbages();

    for (int i = 0; i < START_CHICKENS; ++i) {
        auto x = GetRandomValue(0, WIDTH - 1);
        auto y = GetRandomValue(0, HEIGHT - 1);

        m_grid[y][x]->agent = std::make_shared<Chicken>();
        m_grid[y][x]->agent->m_field = m_grid[y][x].get();
        m_agents.push_back(m_grid[y][x]->agent);
    }

    for (int i = 0; i < START_WOLVES; ++i) {
        auto x = GetRandomValue(0, WIDTH - 1);
        auto y = GetRandomValue(0, HEIGHT - 1);

        m_grid[y][x]->agent = std::make_shared<Wolf>();
        m_grid[y][x]->agent->m_field = m_grid[y][x].get();
        m_agents.push_back(m_grid[y][x]->agent);
    }
}

Simulation::~Simulation() = default;

void Simulation::update() {
    if (GetTime() - m_last_cabbages_spawn >= CABBAGE_SPAWN_TIME) {
        spawn_random_cabbages();
        m_last_cabbages_spawn = GetTime();
    }
    //    for (int y = 0; y < HEIGHT; ++y) {
    //        for (int x = 0; x < WIDTH; ++x) {
    //            auto& field = m_grid[y][x];
    //            if (field->agent_need_update()) {
    //                update_field(*field);
    //            }
    //        }
    //    }


    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_agents.begin(), m_agents.end(), g);
    
    std::vector<std::shared_ptr<Agent>> offsprings;
    for (auto& agent : m_agents) {
        if (agent->need_update()) {
            update_field(*agent->m_field, offsprings);
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
            field->draw();
        }
    }
}

void Simulation::update_field(Field& field, std::vector<std::shared_ptr<Agent>>& offsprings) {
    auto& agent = field.agent;
    if (!agent->is_alive()) {
        agent.reset();
        return;
    }

    auto surr = surroundings(field, agent->sensor());
    agent->update(surr, field, offsprings);
}

std::vector<Field*> Simulation::surroundings(const Field& field, int sensor) {
    auto x = field.get_pos().first;
    auto y = field.get_pos().second;
    std::vector<Field*> tmp;
    for (int f_y = std::max(0, y - sensor); f_y <= std::min(HEIGHT - 1, y + sensor); ++f_y) {
        for (int f_x = std::max(0, x - sensor); f_x <= std::min(WIDTH - 1, x + sensor); ++f_x) {
            if (x != f_x || y != f_y) {
                tmp.push_back(m_grid[f_y][f_x].get());
            }
        }
    }
    return tmp;
}

void Simulation::spawn_random_cabbages() {
    for (int i = 0; i < START_CABBAGES; ++i) {
        auto x = GetRandomValue(0, WIDTH - 1);
        auto y = GetRandomValue(0, HEIGHT - 1);

        auto& field = m_grid[y][x];
        if (field->is_empty()) {
            field->agent = std::make_shared<Cabbage>();
            m_grid[y][x]->agent->m_field = m_grid[y][x].get();
            m_agents.push_back(m_grid[y][x]->agent);
        }
    }
}
