//
// Robot.cpp - Robot agent with battery, collision, and autonomy
//
#include "Simulation/Robot.h"
#include <iostream>
#include <random>

Robot::Robot(int id, int startX, int startY)
    : m_id(id), m_x(startX), m_y(startY)
{
    PickRandomDirection();
    std::cout << m_id << ". Robot created at " << m_x << "," << m_y << std::endl;
}

void Robot::Update(float dt, const Grid& grid, const std::vector<Robot>& allRobots) {
    // Recharge if on a ChargingStation
    if (grid.GetCell(m_x, m_y) == CellType::ChargingStation) {
        m_battery = std::min(100.0f, m_battery + 20.0f * dt);
    }

    // No movement if battery is dead or not autonomous
    if (m_battery <= 0.0f || !m_autonomous)
        return;

    m_moveTimer += dt;
    if (m_moveTimer >= m_moveInterval) {
        m_moveTimer -= m_moveInterval;
        TryMove(m_dirX, m_dirY, grid, allRobots);
    }
}

void Robot::TryMove(int dx, int dy, const Grid& grid, const std::vector<Robot>& allRobots) {
    if (m_battery <= 0.0f)
        return;

    int targetX = m_x + dx;
    int targetY = m_y + dy;

    // Boundary and wall check
    if (grid.GetCell(targetX, targetY) == CellType::Wall) {
        std::cout << "Robot " << m_id << " blocked by Wall at " << targetX << "," << targetY << std::endl;
        PickRandomDirection();
        return;
    }

    // Robot-to-robot collision check
    if (IsOccupiedByOtherRobot(targetX, targetY, allRobots)) {
        std::cout << "Robot " << m_id << " blocked by another Robot at " << targetX << "," << targetY << std::endl;
        PickRandomDirection();
        return;
    }

    m_x = targetX;
    m_y = targetY;
    m_battery = std::max(0.0f, m_battery - 0.5f);
}

bool Robot::IsOccupiedByOtherRobot(int x, int y, const std::vector<Robot>& allRobots) const {
    for (const auto& other : allRobots) {
        if (other.getId() == m_id)
            continue;
        if (other.getX() == x && other.getY() == y)
            return true;
    }
    return false;
}

void Robot::PickRandomDirection() {
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_int_distribution<int> dist(0, 3);
    static const int dirs[4][2] = {{0,-1},{0,1},{-1,0},{1,0}};
    int pick = dist(rng);
    m_dirX = dirs[pick][0];
    m_dirY = dirs[pick][1];
}

int Robot::getX() const { return m_x; }
int Robot::getY() const { return m_y; }
int Robot::getId() const { return m_id; }
float Robot::getBattery() const { return m_battery; }
bool Robot::isAutonomous() const { return m_autonomous; }

void Robot::setX(int x) { m_x = x; }
void Robot::setY(int y) { m_y = y; }
void Robot::setAutonomous(bool autonomous) { m_autonomous = autonomous; }