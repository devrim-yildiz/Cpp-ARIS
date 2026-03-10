//
// Robot.h - Robot agent with battery, collision, autonomy, A* pathfinding, and smooth lerping
//

#pragma once
#include "Simulation/Grid.h"
#include <SFML/System/Vector2.hpp>
#include <vector>

class Robot {
public:
    Robot(int id, int startX, int startY);

    void Update(float dt, const Grid& grid, const std::vector<Robot>& allRobots);
    void TryMove(int dx, int dy, const Grid& grid, const std::vector<Robot>& allRobots);

    // Getters
    int getX() const;
    int getY() const;
    int getId() const;
    float getBattery() const;
    bool isAutonomous() const;

    // Smooth render position (interpolated between previous and current grid cell)
    float getRenderX() const;
    float getRenderY() const;

    // Setters
    void setX(int x);
    void setY(int y);
    void setAutonomous(bool autonomous);

    // A* pathfinding target
    void SetPathTarget(sf::Vector2i target, const Grid& grid);
    bool hasPath() const;

private:
    int m_id;
    int m_x;
    int m_y;
    float m_battery = 100.0f;
    bool m_autonomous = true;
    float m_moveTimer = 0.0f;
    float m_moveInterval = 0.6f;
    int m_dirX = 1;
    int m_dirY = 0;

    // Smooth render lerping
    float m_prevX;
    float m_prevY;
    float m_lerpT = 1.0f; // 0 = at prev pos, 1 = at current pos

    // A* path following
    std::vector<sf::Vector2i> m_path;
    int m_pathIndex = 0;

    bool IsOccupiedByOtherRobot(int x, int y, const std::vector<Robot>& allRobots) const;
    void PickRandomDirection();
    void FollowPath(const Grid& grid, const std::vector<Robot>& allRobots);
    void ApplyMove(int newX, int newY);
};
