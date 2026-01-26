#pragma once
#include <vector>
#include <queue>
#include <unordered_map>
#include "Core/Utils.h"
#include "Simulation/Grid.h"

class Robot;

class Pathfinder {
public:
    Pathfinder();
    
    std::vector<Utils::Point> FindPath(
        const Utils::Point& start,
        const Utils::Point& goal,
        const Grid& grid,
        const std::vector<Robot>& robots,
        int excludeRobotId = -1
    );
    
    bool IsPathClear(
        const Utils::Point& from,
        const Utils::Point& to,
        const Grid& grid,
        const std::vector<Robot>& robots,
        int excludeRobotId = -1
    );

private:
    struct Node {
        Utils::Point pos;
        int g;
        int h;
        int f;
        Utils::Point parent;
        
        Node() : pos(), g(0), h(0), f(0), parent(-1, -1) {}
        Node(const Utils::Point& p, int g_, int h_, const Utils::Point& par)
            : pos(p), g(g_), h(h_), f(g_ + h_), parent(par) {}
    };
    
    struct NodeComparator {
        bool operator()(const Node& a, const Node& b) const {
            return a.f > b.f;
        }
    };
    
    bool IsWalkable(int x, int y, const Grid& grid, const std::vector<Robot>& robots, int excludeRobotId) const;
    std::vector<Utils::Point> ReconstructPath(
        const std::unordered_map<int, Utils::Point>& cameFrom,
        const Utils::Point& current,
        int gridWidth
    ) const;
    int GetHash(const Utils::Point& p, int width) const { return p.y * width + p.x; }
};
