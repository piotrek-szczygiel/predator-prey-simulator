#pragma once
#include <raylib.h>
#include <cmath>
#include <deque>
#include "agent.h"

struct ChunkUpdate {
    int old_chunk;
    int new_chunk;
    Agent* agent;
};

class Map {
   public:
    Map(Vec2 map_size, Vec2 chunk_size)
        : m_chunk_size(chunk_size),
          m_chunk_count(
              {(int)std::ceil((double)map_size.x / chunk_size.x), (int)std::ceil((double)map_size.y / chunk_size.y)}) {
        clear();
    }

    std::vector<std::deque<Agent>>& chunks() { return m_chunks; }
    Vec2 chunk_count() const { return m_chunk_count; }
    Vec2 chunk_size() const { return m_chunk_size; }

    void clear();
    void remove_dead();
    void move(Agent* agent, Vec2 pos);

    Agent* add(AgentType type, Vec2 pos, int energy, Tick last_update);
    int count(AgentType type) const;

    SmallVector<Agent*> get_nearby_to(const Agent* agent);
    SmallVector<Agent*> update_chunks();

   private:
    std::vector<std::deque<Agent>> m_chunks{};
    std::vector<ChunkUpdate> m_chunk_updates{};

    Vec2 m_chunk_count;
    Vec2 m_chunk_size;

    int get_chunk(Vec2 pos) const;
};