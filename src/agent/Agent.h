#pragma once

#include <raylib.h>
#include <memory>
#include <vector>

#include "../map/Field.h"

enum class AgentType { CABBAGE, CHICKEN, WOLF };

constexpr double BASE_METRIC_VALUE{999};
constexpr float BASE_ENERGY_VALUE{9.0f};
constexpr float MAX_ENERGY_VALUE{20.0f};
constexpr float DYNAMIC_AGENT_UPDATE_TIME {0.1f};

struct HeatField {
    int m_x;
    int m_y;
    double distance{BASE_METRIC_VALUE};
    double ca_dist{BASE_METRIC_VALUE};
    double ch_dist{BASE_METRIC_VALUE};
    double fo_dist{BASE_METRIC_VALUE};

    HeatField(int x, int y) : m_x{x}, m_y{y} {}
    std::pair<int, int> get_pos() { return {m_x, m_y}; }
};

class Field;

class Agent {
   public:
    explicit Agent(AgentType type, int sensor, float energy)
        : m_type{type}, m_last_update{GetTime()}, m_texture{}, m_sensor{sensor}, m_energy{energy}, m_field{} {}
    virtual ~Agent() = 0;
    virtual void draw(int x, int y) = 0;
    virtual void eat(std::shared_ptr<Agent>& prey);
    virtual bool need_update();
    virtual std::pair<int, int> calculate_move(std::vector<std::shared_ptr<Field>> surroundings, std::shared_ptr<Field> start_field) = 0;
    AgentType get_type() { return m_type; }
    int sensor() { return m_sensor; }
    bool is_alive() { return m_energy > 0; }
    float get_energy() { return m_energy; }
    bool want_to_breed() const { return m_energy / MAX_ENERGY_VALUE >= 0.59f; }
    void set_energy(float energy) { m_energy = energy; }
    void reduce_energy(float divider) { m_energy *= divider; }
    void set_field(const std::shared_ptr<Field>& field) { m_field = field; }
    std::shared_ptr<Field> get_field() {return m_field;}

   protected:
    std::shared_ptr<Texture2D> m_texture;
    std::shared_ptr<Field> m_field;
    double m_last_update;
    int m_sensor;
    AgentType m_type;
    float m_energy;

    virtual double calculate_metric(std::shared_ptr<HeatField> field) const = 0;
};

inline Agent::~Agent() = default;