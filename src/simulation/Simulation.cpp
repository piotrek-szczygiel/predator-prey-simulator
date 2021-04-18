#include "Simulation.h"
#include <utility>

#include "../Util.h"

Simulation::Simulation() : m_last_cabbages_spawn{GetTime()} {
    grass_texture = std::make_shared<Texture2D>(LoadTexture("../assets/ground.png"));

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            m_grid[y][x] = std::make_shared<Field>(x, y);
        }
    }

    spawn_random_cabbages();

    for (int i = 0; i < START_CHICKENS + 10; ++i) {
        auto x = GetRandomValue(0, WIDTH - 1);
        auto y = GetRandomValue(0, HEIGHT - 1);

        m_grid[y][x]->agent = std::make_shared<Chicken>();
    }

    for (int i = 0; i < START_WOLVES; ++i) {
        auto x = GetRandomValue(0, WIDTH - 1);
        auto y = GetRandomValue(0, HEIGHT - 1);

        m_grid[y][x]->agent = std::make_shared<Wolf>();
    }
}

Simulation::~Simulation() = default;

void Simulation::update() {
    if (GetTime() - m_last_cabbages_spawn >= CABBAGE_SPAWN_TIME) {
        spawn_random_cabbages();
        m_last_cabbages_spawn = GetTime();
    }
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            auto& field = m_grid[y][x];
            if (field->agent_need_update()) {
                update_field(field);
            }
        }
    }
}

void Simulation::draw() {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            DrawTextureV(*grass_texture, convert_to_pos(x, y), RAYWHITE);
        }
    }

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            const auto& field = m_grid[y][x];
            field->draw();
        }
    }
}

void Simulation::update_field(std::shared_ptr<Field>& field) {
    auto x = field->get_pos().first;
    auto y = field->get_pos().second;
    auto& agent = field->agent;
    if (!agent->is_alive()) {
        agent.reset();
        return;
    }

    auto surr = surroundings(x, y, agent->sensor());
    auto new_pos = agent->calculate_move(surr, field);
    auto target_field = m_grid[new_pos.second][new_pos.first];
    if (target_field != field) {
        auto& target_agent = target_field->agent;
        if (target_field->empty()) {
            if (agent->get_type() == AgentType::CABBAGE) {
                target_agent = std::make_shared<Cabbage>();
            } else {
                target_agent = agent;
                field->agent.reset();
            }
        } else {
            if (agent->get_type() == AgentType::CHICKEN) {
                if (target_agent->get_type() == AgentType::CHICKEN) {
                    breed(field, target_field);
                } else if (target_agent->get_type() == AgentType::CABBAGE) {
                    move_and_eat(agent, target_agent);
                }
            } else if (agent->get_type() == AgentType::WOLF) {
                if (target_agent->get_type() == AgentType::WOLF) {
                    breed(field, target_field);
                } else {
                    move_and_eat(agent, target_agent);
                }
            }
        }
    }
}

std::vector<std::shared_ptr<Field>> Simulation::surroundings(int x, int y, int sensor) {
    std::vector<std::shared_ptr<Field>> tmp;
    for (int f_y = std::max(0, y - sensor); f_y <= std::min(HEIGHT - 1, y + sensor); ++f_y) {
        for (int f_x = std::max(0, x - sensor); f_x <= std::min(WIDTH - 1, x + sensor); ++f_x) {
            if (x != f_x || y != f_y) {
                tmp.push_back(m_grid[f_y][f_x]);
            }
        }
    }
    return tmp;
}

void Simulation::move_and_eat(std::shared_ptr<Agent>& agent, std::shared_ptr<Agent>& target_agent) {
    if ((agent->get_type() == AgentType::CHICKEN && target_agent->get_type() == AgentType::CABBAGE) ||
        (agent->get_type() == AgentType::WOLF && target_agent->get_type() == AgentType::CHICKEN)) {
        agent->eat(target_agent);
    }
    target_agent = agent;
    agent.reset();
}

void Simulation::breed(std::shared_ptr<Field>& field, std::shared_ptr<Field>& target_field) {
    auto offspring_field = nearest_breeding_field(field, target_field);
    auto agent = field->agent;
    auto target_agent = target_field->agent;
    if (offspring_field) {
        auto energy = (agent->get_energy() + target_agent->get_energy()) / 4;
        agent->reduce_energy(0.5f);
        target_agent->reduce_energy(0.5f);
        switch (agent->get_type()) {
            case AgentType::CHICKEN: offspring_field->agent = std::make_shared<Chicken>(energy); break;
            case AgentType::WOLF: offspring_field->agent = std::make_shared<Wolf>(energy); break;
            case AgentType::CABBAGE:
            default: break;
        }
    }
}

std::shared_ptr<Field> Simulation::nearest_breeding_field(const std::shared_ptr<Field>& first_field,
                                                          const std::shared_ptr<Field>& second_field) {
    auto surr = surroundings(first_field->get_pos().first, first_field->get_pos().second, 1);
    auto second_surr = surroundings(second_field->get_pos().first, second_field->get_pos().second, 1);
    surr.insert(surr.end(), second_surr.begin(), second_surr.end());

    for (auto& field : surr) {
        if (field->empty() || (!field->empty() && field->agent->get_type() == AgentType::CABBAGE)) {
            return field;
        }
    }

    return std::shared_ptr<Field>();
}

void Simulation::spawn_random_cabbages() {
    for (int i = 0; i < START_CABBAGES; ++i) {
        auto x = GetRandomValue(0, WIDTH - 1);
        auto y = GetRandomValue(0, HEIGHT - 1);

        auto& field = m_grid[y][x];
        if (field->empty()) {
            field->agent = std::make_shared<Cabbage>();
        }
    }
}
