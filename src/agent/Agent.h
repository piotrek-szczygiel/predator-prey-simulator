#pragma once

#include <raylib.h>
#include <map>
#include <memory>
#include <vector>

#include "../map/Field.h"
#include "../simulation/SimulationSettings.h"

enum class AgentType { CABBAGE, CHICKEN, WOLF };

class Field;

class Agent {
   public:
    explicit Agent(AgentType type, int sensor, int energy)
        : m_type{type}, m_last_update{GetTime()}, m_texture{}, m_sensor{sensor}, m_energy{energy}, eaten{false} {}
    virtual ~Agent() = 0;
    virtual void draw(int x, int y) = 0;
    virtual void update(Field map[HEIGHT][WIDTH], std::vector<std::shared_ptr<Agent>>& offsprings);
    AgentType get_type() const { return m_type; }
    int sensor() { return m_sensor; }
    bool is_alive() { return m_energy > 0; }
    int get_energy() { return m_energy; }
    bool want_to_breed() const { return m_energy >= 5900; }
    void set_energy(int energy) { m_energy = energy; }
    void reduce_energy(int divider) { m_energy /= divider; }
    void set_field(Field* field) { m_field = field; }
    Field* get_field() { return m_field; }
    int convert_energy(Agent& partner);
    bool was_eaten() { return eaten; }

   protected:
    Texture2D m_texture;
    double m_last_update;
    int m_sensor;
    int m_energy;
    bool eaten;
    AgentType m_type;
    Field* m_field;

    bool field_in_range(const Field& target, const Field& start, int range);
    virtual void apply_field_metrics(Field* field, const Agent& compare_agent, int distance) const = 0;
    virtual int calculate_metric(const Field* field) const = 0;
    virtual void eat(Agent& prey);
    virtual void apply_behaviour(Field map[HEIGHT][WIDTH], std::vector<std::shared_ptr<Agent>>& offsprings,
                                 Field& current_field, Field* target) = 0;
    void move_to_field(Field& current_field, Field* target);
};

inline Agent::~Agent() = default;