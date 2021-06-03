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

struct Node {
    Vec2 parent;
    int g_cost;
    int f_cost;

    bool closed;

    Node() : parent({-1, -1}), g_cost(-1), f_cost(-1), closed(false) {}
};

class Pathfinder {
   public:
    explicit Pathfinder(Vec2 map_size) : m_map_size(map_size) { m_nodes.resize((size_t)map_size.x * map_size.y); }

    Vec2 get_next_step(Vec2 start, Vec2 target, const Grid& grid);

   private:
    std::vector<Node> m_nodes;
    Vec2 m_map_size;

    Node& at(Vec2 pos) { return m_nodes[(size_t)m_map_size.x * pos.y + pos.x]; }
    bool is_valid(Vec2 pos) const;
    static bool is_blocked(Vec2 pos, const Grid& grid);
    Vec2 trace(Vec2 start, Vec2 target);
};