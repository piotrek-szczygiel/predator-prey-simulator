#include <queue>
#include <tuple>
#include <vector>
#include "agent.h"
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
    explicit Pathfinder(Vec2 map_size) : m_map_size(map_size) { m_nodes.resize(map_size.x * map_size.y); }

    Vec2 get_next_step(Vec2 start, Vec2 target, const std::vector<std::vector<Agent*>>& grid);

   private:
    std::vector<Node> m_nodes;
    Vec2 m_map_size;

    int node_id(Vec2 pos) const;
    bool is_valid(Vec2 pos) const;
    static bool is_blocked(Vec2 pos, const std::vector<std::vector<Agent*>>& grid);
    Vec2 trace(Vec2 start, Vec2 target);
};