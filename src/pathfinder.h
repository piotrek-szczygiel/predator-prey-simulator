#include <queue>
#include <tuple>
#include <vector>
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

    bool blocked;
    bool target;

    Node() : parent({-1, -1}), g_cost(-1), f_cost(-1), blocked(false), target(false) {}
};

class Pathfinder {
   public:
    Pathfinder(int sensor, int map_width, int map_height, int start_x, int start_y);
    void add_blocker(Vec2 blocker);
    Vec2 get_next_step(Vec2 target);

   private:
    std::vector<std::vector<Node>> m_nodes;

    Vec2 m_start;
    Vec2 m_lower_bound;
    Vec2 m_segment_size;

    Vec2 trace(Vec2 target);
};
