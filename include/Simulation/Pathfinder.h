//
// Pathfinder.h - A* pathfinding on the Grid
//

#pragma once
#include "Simulation/Grid.h"
#include <SFML/System/Vector2.hpp>
#include <vector>

class Pathfinder {
public:
    // Find a path from start to goal on the given grid.
    // Returns an ordered list of grid coordinates from start to goal (inclusive).
    // Returns an empty vector if no path exists.
    static std::vector<sf::Vector2i> FindPath(const Grid& grid,
                                               sf::Vector2i start,
                                               sf::Vector2i goal);

private:
    // Returns true if (x, y) is walkable (not a wall and within bounds)
    static bool IsWalkable(const Grid& grid, int x, int y);

    // Manhattan distance heuristic
    static int Heuristic(sf::Vector2i a, sf::Vector2i b);
};
