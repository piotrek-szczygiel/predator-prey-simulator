#include "pathfinder.h"
#include <cstring>

Vec2 Pathfinder::get_next_step(Vec2 start, Vec2 target, const Grid& grid) {
    std::memset(&m_nodes[0], 0, sizeof(PathfinderNode) * m_nodes.size());

    at(start).f_cost = 1;
    at(start).g_cost = 1;
    at(start).parent = start;

    std::priority_queue<AStarNode, std::vector<AStarNode>, AStarNodeComparator> open_queue;

    open_queue.push({1, start});
    while (!open_queue.empty()) {
        Vec2 p = open_queue.top().pos;
        open_queue.pop();

        at(p).visited = true;

        Vec2 available_movements[] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
        for (const auto& movement : available_movements) {
            Vec2 neighbour = p + movement;

            if (is_valid(neighbour)) {
                if (neighbour == target) {
                    at(neighbour).parent = p;
                    return trace(start, target);
                } else if (!at(neighbour).visited && !is_blocked(neighbour, grid)) {
                    int g_new = at(p).g_cost + 1;
                    int f_new = g_new + distance(target, neighbour);

                    if (at(neighbour).f_cost == 0 || f_new < at(neighbour).f_cost) {
                        open_queue.push({f_new, neighbour});
                        at(neighbour).g_cost = g_new;
                        at(neighbour).f_cost = f_new;
                        at(neighbour).parent = p;
                    }
                }
            }
        }
    }

    return {0, 0};
}

Vec2 Pathfinder::trace(Vec2 start, Vec2 target) {
    Vec2 current = target;
    Vec2 parent = at(current).parent;

    while (parent != start) {
        current = parent;
        parent = at(current).parent;
    }

    return current - start;
}