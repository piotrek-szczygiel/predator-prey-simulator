#include <queue>
#include <tuple>
#include <vector>
#include "agent.h"
#include "util.h"

struct AStarNode {
    int f_cost;
    Vec2 position;
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
    Pathfinder(int map_width, int map_height);
    Vec2 get_next_step(Vec2 start, Vec2 target, const std::vector<std::vector<Agent*>>& grid);

   private:
    std::vector<Node> m_nodes;

    Vec2 m_map_size;

    int node_id(Vec2 pos) const;
    bool is_valid(Vec2 pos) const;
    bool is_blocked(Vec2 pos, const std::vector<std::vector<Agent*>>& grid) const;
    Vec2 trace(Vec2 start, Vec2 target);
};
