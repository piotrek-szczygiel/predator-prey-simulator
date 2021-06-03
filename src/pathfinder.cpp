#include "pathfinder.h"

int Pathfinder::node_id(Vec2 pos) const {
    return m_map_size.x * pos.y + pos.x;
}

bool Pathfinder::is_valid(Vec2 pos) const {
    return pos.x >= 0 && pos.x < m_map_size.x && pos.y >= 0 && pos.y < m_map_size.y;
}

bool Pathfinder::is_blocked(Vec2 pos, const Grid& grid) {
    auto agent = grid.at(pos);
    return agent && agent->type != AgentType::Grass;
}

Vec2 Pathfinder::get_next_step(Vec2 start, Vec2 target, const Grid& grid) {
    m_nodes.assign(m_nodes.size(), Node{});

    int start_id = node_id(start);
    m_nodes[start_id].f_cost = 0;
    m_nodes[start_id].g_cost = 0;
    m_nodes[start_id].parent = {start.x, start.y};

    std::priority_queue<AStarNode, std::vector<AStarNode>, AStarNodeComparator> open_queue;

    open_queue.push({0, start});
    while (!open_queue.empty()) {
        Vec2 p = open_queue.top().pos;
        open_queue.pop();

        m_nodes[node_id(p)].closed = true;

        Vec2 available_movements[] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

        for (const auto& movement : available_movements) {
            Vec2 neighbour{p.x + movement.x, p.y + movement.y};

            if (is_valid(neighbour)) {
                if (neighbour.x == target.x && neighbour.y == target.y) {
                    m_nodes[node_id(neighbour)].parent = {p.x, p.y};
                    return trace(start, target);
                } else if (!m_nodes[node_id(neighbour)].closed && !is_blocked(neighbour, grid)) {
                    int g_new = m_nodes[node_id(p)].g_cost + 1;
                    int f_new = g_new + distance(target, neighbour);

                    if (m_nodes[node_id(neighbour)].f_cost == -1 || m_nodes[node_id(neighbour)].f_cost > f_new) {
                        open_queue.push({f_new, neighbour});

                        m_nodes[node_id(neighbour)].g_cost = g_new;
                        m_nodes[node_id(neighbour)].f_cost = f_new;

                        m_nodes[node_id(neighbour)].parent = {p.x, p.y};
                    }
                }
            }
        }
    }

    return {0, 0};
}

Vec2 Pathfinder::trace(Vec2 start, Vec2 target) {
    Vec2 current = {target.x, target.y};
    Vec2 parent = m_nodes[node_id(current)].parent;

    while (!(parent.x == start.x && parent.y == start.y)) {
        current = parent;
        parent = m_nodes[node_id(current)].parent;
    }

    int dx = current.x - start.x;
    int dy = current.y - start.y;
    return {dx, dy};
}