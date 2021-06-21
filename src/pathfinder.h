#include <queue>
#include <tuple>
#include <vector>
#include "agent.h"
#include "grid.h"
#include "util.h"

struct AStarNode {
    int f_cost;
    Vec2 pos;
};

struct AStarNodeComparator {
    bool operator()(const AStarNode& lhs, const AStarNode& rhs) { return lhs.f_cost > rhs.f_cost; }
};

struct PathfinderNode {
    Vec2 parent;
    int g_cost;
    int f_cost;
    bool visited;
};

class Pathfinder {
   public:
    explicit Pathfinder(Vec2 map_size) : m_map_size(map_size) { m_nodes.resize((size_t)map_size.x * map_size.y); }
    Vec2 get_next_step(Vec2 start, Vec2 target, const Grid& grid);

   private:
    std::vector<PathfinderNode> m_nodes;
    Vec2 m_map_size;

    inline PathfinderNode& at(Vec2 pos) { return m_nodes[(size_t)m_map_size.x * pos.y + pos.x]; }

    inline bool is_valid(Vec2 pos) const {
        return pos.x >= 0 && pos.x < m_map_size.x && pos.y >= 0 && pos.y < m_map_size.y;
    }

    static inline bool is_blocked(Vec2 pos, const Grid& grid) {
        return grid.at(pos) && grid.at(pos)->type != AgentType::Grass;
    }

    Vec2 trace(Vec2 start, Vec2 target);
};
