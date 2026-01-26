#include "Simulation/Pathfinder.h"
#include "Simulation/Robot.h"
#include <unordered_set>
#include <algorithm>

Pathfinder::Pathfinder() {}

std::vector<Utils::Point> Pathfinder::FindPath(
    const Utils::Point& start,
    const Utils::Point& goal,
    const Grid& grid,
    const std::vector<Robot>& robots,
    int excludeRobotId
) {
    if (start == goal) return { goal };
    if (!IsWalkable(goal.x, goal.y, grid, robots, excludeRobotId)) return {};

    std::priority_queue<Node, std::vector<Node>, NodeComparator> openSet;
    std::unordered_set<int> closedSet;
    std::unordered_map<int, Utils::Point> cameFrom;
    std::unordered_map<int, int> gScore;
    
    int width = grid.GetWidth();
    int startHash = GetHash(start, width);
    int goalHash = GetHash(goal, width);
    
    gScore[startHash] = 0;
    int h = Utils::ManhattanDistance(start, goal);
    openSet.push(Node(start, 0, h, Utils::Point(-1, -1)));
    
    const int dx[] = {0, 1, 0, -1};
    const int dy[] = {-1, 0, 1, 0};
    
    while (!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();
        
        int currentHash = GetHash(current.pos, width);
        
        if (current.pos == goal) {
            return ReconstructPath(cameFrom, current.pos, width);
        }
        
        if (closedSet.count(currentHash)) continue;
        closedSet.insert(currentHash);
        
        for (int i = 0; i < 4; ++i) {
            int nx = current.pos.x + dx[i];
            int ny = current.pos.y + dy[i];
            Utils::Point neighbor(nx, ny);
            int neighborHash = GetHash(neighbor, width);
            
            if (closedSet.count(neighborHash)) continue;
            if (!IsWalkable(nx, ny, grid, robots, excludeRobotId)) continue;
            
            int tentativeG = current.g + 1;
            
            if (!gScore.count(neighborHash) || tentativeG < gScore[neighborHash]) {
                gScore[neighborHash] = tentativeG;
                cameFrom[neighborHash] = current.pos;
                int hScore = Utils::ManhattanDistance(neighbor, goal);
                openSet.push(Node(neighbor, tentativeG, hScore, current.pos));
            }
        }
    }
    
    return {};
}

bool Pathfinder::IsPathClear(
    const Utils::Point& from,
    const Utils::Point& to,
    const Grid& grid,
    const std::vector<Robot>& robots,
    int excludeRobotId
) {
    auto path = FindPath(from, to, grid, robots, excludeRobotId);
    return !path.empty();
}

bool Pathfinder::IsWalkable(int x, int y, const Grid& grid, const std::vector<Robot>& robots, int excludeRobotId) const {
    if (!grid.IsWalkable(x, y)) return false;
    
    for (const auto& robot : robots) {
        if (robot.getId() != excludeRobotId && robot.getX() == x && robot.getY() == y) {
            return false;
        }
    }
    
    return true;
}

std::vector<Utils::Point> Pathfinder::ReconstructPath(
    const std::unordered_map<int, Utils::Point>& cameFrom,
    const Utils::Point& current,
    int gridWidth
) const {
    std::vector<Utils::Point> path;
    Utils::Point pos = current;
    path.push_back(pos);
    
    while (true) {
        int hash = GetHash(pos, gridWidth);
        if (!cameFrom.count(hash)) break;
        pos = cameFrom.at(hash);
        if (pos.x == -1 && pos.y == -1) break;
        path.push_back(pos);
    }
    
    std::reverse(path.begin(), path.end());
    return path;
}
