#include "pathfinder.h"
#include <cstdio>
#include "util.h"

Pathfinder::Pathfinder(int sensor, int map_width, int map_height, int start_x, int start_y)
    : start({start_x, start_y}) {
    x_lower_bound = std::max(0, start_x - sensor);
    y_lower_bound = std::max(0, start_y - sensor);

    int x_upper_bound = std::min(start_x + sensor, map_width);
    int y_upper_bound = std::min(start_y + sensor, map_height);

    segment_width = x_upper_bound - x_lower_bound + 1;
    segment_height = y_upper_bound - y_lower_bound + 1;

    nodes.resize(segment_height, std::vector<Node>(segment_width));
}

void Pathfinder::add_blocker(int x, int y) {
    nodes[y - y_lower_bound][x - x_lower_bound].blocked = true;
}

std::pair<int, int> Pathfinder::get_next_step(int target_x, int target_y) {
    std::vector<std::vector<bool>> closed;
    closed.resize(segment_height, std::vector<bool>(segment_width, false));

    int shifted_tx = target_x - x_lower_bound;
    int shifted_ty = target_y - y_lower_bound;
    nodes[shifted_ty][shifted_tx].target = true;

    int i = start.first - x_lower_bound;
    int j = start.second - y_lower_bound;

    nodes[j][i].f_cost = 0;
    nodes[j][i].g_cost = 0;
    nodes[j][i].parent = {i, j};

    std::priority_queue<Tuple, std::vector<Tuple>, std::greater<>> open_queue;

    open_queue.emplace(0, i, j);
    while (!open_queue.empty()) {
        const Tuple& p = open_queue.top();

        i = std::get<1>(p);
        j = std::get<2>(p);

        open_queue.pop();
        closed[j][i] = true;

        Pair available_movements[] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

        for (const auto& movement : available_movements) {
            Pair neighbour(i + movement.first, j + movement.second);

            if (neighbour.first >= 0 && neighbour.first < segment_width && neighbour.second >= 0 &&
                neighbour.second < segment_height) {
                if (nodes[neighbour.second][neighbour.first].target) {
                    nodes[neighbour.second][neighbour.first].parent = {i, j};
                    return trace(target_x, target_y);
                } else if (!closed[neighbour.second][neighbour.first] &&
                           !nodes[neighbour.second][neighbour.first].blocked) {
                    int g_new = nodes[i][j].g_cost + 1;
                    int f_new = g_new + distance({shifted_tx, shifted_ty}, neighbour);

                    if (nodes[neighbour.second][neighbour.first].f_cost == -1 ||
                        nodes[neighbour.second][neighbour.first].f_cost > f_new) {
                        open_queue.emplace(f_new, neighbour.first, neighbour.second);

                        nodes[neighbour.second][neighbour.first].g_cost = g_new;
                        nodes[neighbour.second][neighbour.first].f_cost = f_new;

                        nodes[neighbour.second][neighbour.first].parent = {i, j};
                    }
                }
            }
        }
    }

    return std::pair<int, int>({0, 0});
}

std::pair<int, int> Pathfinder::trace(int target_x, int target_y) {
    Pair current = {target_x - x_lower_bound, target_y - y_lower_bound};
    Pair parent = nodes[current.second][current.first].parent;

    while (!(parent.first == start.first - x_lower_bound && parent.second == start.second - y_lower_bound)) {
        current = parent;
        parent = nodes[current.second][current.first].parent;
    }

    int dx = current.first + x_lower_bound - start.first;
    int dy = current.second + y_lower_bound - start.second;
    return std::pair<int, int>({dx, dy});
}

void Pathfinder::debug_print() {
    printf("dimensions: %llu\t%llu\n", nodes.size(), nodes[0].size());
    for (auto y = 0; y < nodes.size(); ++y) {
        for (auto x = 0; x < nodes[y].size(); ++x) {
            printf("%d ", nodes[y][x].f_cost);
        }
        printf("\n");
    }
}
