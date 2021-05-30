#include "pathfinder.h"

Pathfinder::Pathfinder(int sensor, int map_width, int map_height, int start_x, int start_y)
    : m_start({start_x, start_y}) {
    m_lower_bound = {std::max(0, start_x - sensor), std::max(0, start_y - sensor)};

    Vec2 upper_bound = {std::min(start_x + sensor, map_width), std::min(start_y + sensor, map_height)};

    m_segment_size = {upper_bound.x - m_lower_bound.x + 1, upper_bound.y - m_lower_bound.y + 1};

    m_nodes.resize(m_segment_size.y, std::vector<Node>(m_segment_size.x));
}

void Pathfinder::add_blocker(Vec2 blocker) {
    m_nodes[blocker.y - m_lower_bound.y][blocker.x - m_lower_bound.x].blocked = true;
}

Vec2 Pathfinder::get_next_step(Vec2 target) {
    std::vector<std::vector<bool>> closed;
    closed.resize(m_segment_size.y, std::vector<bool>(m_segment_size.x, false));

    Vec2 s_target{target.x - m_lower_bound.x, target.y - m_lower_bound.y};
    m_nodes[s_target.y][s_target.x].target = true;

    Vec2 s_start{m_start.x - m_lower_bound.x, m_start.y - m_lower_bound.y};
    m_nodes[s_start.y][s_start.x].f_cost = 0;
    m_nodes[s_start.y][s_start.x].g_cost = 0;
    m_nodes[s_start.y][s_start.x].parent = {s_start.x, s_start.y};

    std::priority_queue<AStarNode, std::vector<AStarNode>, AStarNodeComparator> open_queue;

    open_queue.push({0, s_start});
    while (!open_queue.empty()) {
        Vec2 p = open_queue.top().position;
        open_queue.pop();

        closed[p.y][p.x] = true;

        Vec2 available_movements[] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

        for (const auto& movement : available_movements) {
            Vec2 neighbour{p.x + movement.x, p.y + movement.y};

            if (neighbour.x >= 0 && neighbour.x < m_segment_size.x && neighbour.y >= 0 &&
                neighbour.y < m_segment_size.y) {
                if (m_nodes[neighbour.y][neighbour.x].target) {
                    m_nodes[neighbour.y][neighbour.x].parent = {p.x, p.y};
                    return trace(target);
                } else if (!closed[neighbour.y][neighbour.x] && !m_nodes[neighbour.y][neighbour.x].blocked) {
                    int g_new = m_nodes[p.y][p.x].g_cost + 1;
                    int f_new = g_new + distance(s_target, neighbour);

                    if (m_nodes[neighbour.y][neighbour.x].f_cost == -1 ||
                        m_nodes[neighbour.y][neighbour.x].f_cost > f_new) {
                        open_queue.push({f_new, neighbour});

                        m_nodes[neighbour.y][neighbour.x].g_cost = g_new;
                        m_nodes[neighbour.y][neighbour.x].f_cost = f_new;

                        m_nodes[neighbour.y][neighbour.x].parent = {p.x, p.y};
                    }
                }
            }
        }
    }

    return {0, 0};
}

Vec2 Pathfinder::trace(Vec2 target) {
    Vec2 current = {target.x - m_lower_bound.x, target.y - m_lower_bound.y};
    Vec2 parent = m_nodes[current.y][current.x].parent;

    while (!(parent.x == m_start.x - m_lower_bound.x && parent.y == m_start.y - m_lower_bound.y)) {
        current = parent;
        parent = m_nodes[current.y][current.x].parent;
    }

    int dx = current.x + m_lower_bound.x - m_start.x;
    int dy = current.y + m_lower_bound.y - m_start.y;
    return {dx, dy};
}
