#pragma once

enum class AgentType { None, Wolf, Chicken, Cabbage };

class Agent {
   public:
    Agent(AgentType type) : type(type) {}
    Agent() : type(AgentType::None) {}

    bool none() { return type == AgentType::None; }

    AgentType type;
    int energy = 0;
};