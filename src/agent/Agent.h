#pragma once

#include <raylib.h>
#include <memory>
#include <vector>

#include "../map/Field.h"

enum class AgentType { CABBAGE, CHICKEN, WOLF };

constexpr float BASE_ENERGY_VALUE{40.0f};
constexpr float MAX_ENERGY_VALUE{100.0f};
constexpr float DYNAMIC_AGENT_UPDATE_TIME{0.1f};

class Field;

class Agent {
   public:
    explicit Agent(AgentType type, int sensor, float energy)
        : m_type{type}, m_last_update{GetTime()}, m_texture{}, m_sensor{sensor}, m_energy{energy}, eaten{false} {}
    virtual ~Agent() = 0;
    virtual void draw(int x, int y) = 0;
    virtual void eat(Agent& prey);
    virtual bool need_update();
    virtual void update(std::vector<Field*>& surroundings, std::vector<std::shared_ptr<Agent>>& offsprings) = 0;
    AgentType get_type() { return m_type; }
    int sensor() { return m_sensor; }
    bool is_alive() { return m_energy > 0; }
    float get_energy() { return m_energy; }
    bool want_to_breed() const { return m_energy / MAX_ENERGY_VALUE >= 0.59f; }
    void set_energy(float energy) { m_energy = energy; }
    void reduce_energy(float divider) { m_energy *= divider; }
    void set_field(Field* field) { m_field = field; }
    Field* get_field() { return m_field; }
    float convert_energy(Agent& partner);
    bool was_eaten() { return eaten; }

   protected:
    Texture2D m_texture;
    double m_last_update;
    int m_sensor;
    float m_energy;
    bool eaten;
    AgentType m_type;
    Field* m_field;

    virtual double calculate_metric(const Field* field) const = 0;
};

inline Agent::~Agent() = default;