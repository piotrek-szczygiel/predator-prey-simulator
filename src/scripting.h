#pragma once

#include "agent.h"

class Simulation;

#define SOL_LUAJIT 1
#include <sol/sol.hpp>

class Scripting {
   public:
    bool init();
    bool load(const std::string &path, Simulation &sim);

    void update_chicken(Agent *chicken);
    void update_wolf(Agent *wolf);

   private:
    sol::state m_lua;

    sol::protected_function m_update_chicken;
    sol::protected_function m_update_wolf;

    bool m_valid = false;
};