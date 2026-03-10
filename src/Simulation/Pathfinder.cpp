//
// Pathfinder.cpp - A* pathfinding on the Grid
//
#include "Simulation/Pathfinder.h"
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <functional>

namespace {
    struct Node {
        sf::Vector2i pos;
        int gCost = 0;   // cost from start
        int fCost = 0;   // gCost + heuristic
    };

    struct NodeCompare {
        bool operator()(const Node& a, const Node& b) const {
            return a.fCost > b.fCost; // min-heap
        }
    };

    // Pack a 2D coordinate into a single integer for use as hash key
    struct Vec2iHash {
        std::size_t operator()(sf::Vector2i v) const {
            auto h1 = std::hash<int>{}(v.x);
            auto h2 = std::hash<int>{}(v.y);
            return h1 ^ (h2 * 2654435761u);
        }
    };
}

bool Pathfinder::IsWalkable(const Grid& grid, int x, int y) {
    CellType cell = grid.GetCell(x, y);
    return cell != CellType::Wall;
}

int Pathfinder::Heuristic(sf::Vector2i a, sf::Vector2i b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<sf::Vector2i> Pathfinder::FindPath(const Grid& grid,
                                                 sf::Vector2i start,
                                                 sf::Vector2i goal)
{
    // Quick-reject: if start or goal is not walkable, no path
    if (!IsWalkable(grid, start.x, start.y) || !IsWalkable(grid, goal.x, goal.y))
        return {};

    // Trivial case
    if (start == goal)
        return { start };

    static const sf::Vector2i directions[4] = {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0}
    };

    std::priority_queue<Node, std::vector<Node>, NodeCompare> openSet;
    std::unordered_map<sf::Vector2i, sf::Vector2i, Vec2iHash> cameFrom;
    std::unordered_map<sf::Vector2i, int, Vec2iHash> gScore;

    gScore[start] = 0;
    openSet.push({start, 0, Heuristic(start, goal)});

    while (!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();

        if (current.pos == goal) {
            // Reconstruct path
            std::vector<sf::Vector2i> path;
            sf::Vector2i step = goal;
            while (step != start) {
                path.push_back(step);
                step = cameFrom[step];
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            return path;
        }

        // Skip stale entries
        if (current.gCost > gScore[current.pos])
            continue;

        for (const auto& dir : directions) {
            sf::Vector2i neighbor = current.pos + dir;

            if (!IsWalkable(grid, neighbor.x, neighbor.y))
                continue;

            int tentativeG = current.gCost + 1;
            auto it = gScore.find(neighbor);
            if (it == gScore.end() || tentativeG < it->second) {
                gScore[neighbor] = tentativeG;
                cameFrom[neighbor] = current.pos;
                int f = tentativeG + Heuristic(neighbor, goal);
                openSet.push({neighbor, tentativeG, f});
            }
        }
    }

    return {}; // No path found
}
