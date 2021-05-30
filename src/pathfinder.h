#include <queue>
#include <tuple>
#include <vector>

typedef std::pair<int, int> Pair;

typedef std::tuple<int, int, int> Tuple;

struct Node {
    Pair parent;
    int g_cost;
    int f_cost;

    bool blocked;
    bool target;

    Node() : parent({-1, -1}), g_cost(-1), f_cost(-1), blocked(false), target(false) {}
};

class Pathfinder {
   public:
    Pathfinder(int sensor, int map_width, int map_height, int start_x, int start_y);
    void add_blocker(int x, int y);
    std::pair<int, int> get_next_step(int target_x, int target_y);
    void debug_print();

   private:
    std::vector<std::vector<Node>> nodes;

    Pair start;

    int x_lower_bound;
    int y_lower_bound;

    int segment_width;
    int segment_height;

    std::pair<int, int> trace(int target_x, int target_y);
};
