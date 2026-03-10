//
// Robot.h - Robot agent with battery, collision, and autonomy
//

#pragma once
#include "Simulation/Grid.h"
#include <vector>

class Robot {
public:
    Robot(int id, int startX, int startY);

    void Update(float dt, const Grid& grid, const std::vector<Robot>& allRobots);
    void TryMove(int dx, int dy, const Grid& grid, const std::vector<Robot>& allRobots);

    int getX() const;
    int getY() const;
    int getId() const;
    float getBattery() const;
    bool isAutonomous() const;

    void setX(int x);
    void setY(int y);
    void setAutonomous(bool autonomous);

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

    bool IsOccupiedByOtherRobot(int x, int y, const std::vector<Robot>& allRobots) const;
    void PickRandomDirection();
};
