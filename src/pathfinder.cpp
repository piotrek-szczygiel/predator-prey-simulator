#include "pathfinder.h"

bool Pathfinder::is_valid(Vec2 pos) const {
    return pos.x >= 0 && pos.x < m_map_size.x && pos.y >= 0 && pos.y < m_map_size.y;
}

bool Pathfinder::is_blocked(Vec2 pos, const Grid& grid) {
    return grid.at(pos) && grid.at(pos)->type != AgentType::Grass;
}

Vec2 Pathfinder::get_next_step(Vec2 start, Vec2 target, const Grid& grid) {
    m_nodes.assign(m_nodes.size(), Node{});

    at(start).f_cost = 0;
    at(start).g_cost = 0;
    at(start).parent = start;

    std::priority_queue<AStarNode, std::vector<AStarNode>, AStarNodeComparator> open_queue;

    open_queue.push({0, start});
    while (!open_queue.empty()) {
        Vec2 p = open_queue.top().pos;
        open_queue.pop();

        at(p).closed = true;

        Vec2 available_movements[] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
        for (const auto& movement : available_movements) {
            Vec2 neighbour = p + movement;

            if (is_valid(neighbour)) {
                if (neighbour == target) {
                    at(neighbour).parent = p;
                    return trace(start, target);
                } else if (!at(neighbour).closed && !is_blocked(neighbour, grid)) {
                    int g_new = at(p).g_cost + 1;
                    int f_new = g_new + distance(target, neighbour);

                    if (at(neighbour).f_cost == -1 || at(neighbour).f_cost > f_new) {
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