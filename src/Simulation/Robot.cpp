//
// Robot.cpp - Robot agent with battery, collision, autonomy, A* pathfinding, and smooth lerping
//
#include "Simulation/Robot.h"
#include "Simulation/Pathfinder.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>

Robot::Robot(int id, int startX, int startY)
    : m_id(id), m_x(startX), m_y(startY),
      m_prevX(static_cast<float>(startX)),
      m_prevY(static_cast<float>(startY))
{
    PickRandomDirection();
    std::cout << m_id << ". Robot created at " << m_x << "," << m_y << std::endl;
}

void Robot::Update(float dt, const Grid& grid, const std::vector<Robot>& allRobots) {
    // Recharge if on a ChargingStation
    if (grid.GetCell(m_x, m_y) == CellType::ChargingStation) {
        m_battery = std::min(100.0f, m_battery + 20.0f * dt);
    }

    // Advance lerp interpolation
    if (m_lerpT < 1.0f) {
        m_lerpT += dt / m_moveInterval;
        if (m_lerpT > 1.0f)
            m_lerpT = 1.0f;
    }

    // No movement if battery is dead or not autonomous
    if (m_battery <= 0.0f || !m_autonomous)
        return;

    m_moveTimer += dt;
    if (m_moveTimer >= m_moveInterval) {
        m_moveTimer -= m_moveInterval;

        if (!m_path.empty() && m_pathIndex < static_cast<int>(m_path.size())) {
            FollowPath(grid, allRobots);
        } else {
            TryMove(m_dirX, m_dirY, grid, allRobots);
        }
    }
}

void Robot::TryMove(int dx, int dy, const Grid& grid, const std::vector<Robot>& allRobots) {
    if (m_battery <= 0.0f)
        return;

    int targetX = m_x + dx;
    int targetY = m_y + dy;

    // Boundary and wall check
    if (grid.GetCell(targetX, targetY) == CellType::Wall) {
        PickRandomDirection();
        return;
    }

    // Robot-to-robot collision check
    if (IsOccupiedByOtherRobot(targetX, targetY, allRobots)) {
        PickRandomDirection();
        return;
    }

    // Store previous position for lerp and move
    ApplyMove(targetX, targetY);
}

void Robot::FollowPath(const Grid& grid, const std::vector<Robot>& allRobots) {
    sf::Vector2i next = m_path[m_pathIndex];
    int dx = next.x - m_x;
    int dy = next.y - m_y;

    int targetX = m_x + dx;
    int targetY = m_y + dy;

    // Check if path is still valid (cell could have changed)
    if (grid.GetCell(targetX, targetY) == CellType::Wall ||
        IsOccupiedByOtherRobot(targetX, targetY, allRobots)) {
        // Path blocked, clear it and fall back to random
        m_path.clear();
        m_pathIndex = 0;
        PickRandomDirection();
        return;
    }

    // Store previous position for lerp and move
    ApplyMove(targetX, targetY);
    m_pathIndex++;

    // Path completed
    if (m_pathIndex >= static_cast<int>(m_path.size())) {
        m_path.clear();
        m_pathIndex = 0;
    }
}

void Robot::ApplyMove(int newX, int newY) {
    m_prevX = static_cast<float>(m_x);
    m_prevY = static_cast<float>(m_y);
    m_lerpT = 0.0f;
    m_x = newX;
    m_y = newY;
    m_battery = std::max(0.0f, m_battery - 0.5f);
}

void Robot::SetPathTarget(sf::Vector2i target, const Grid& grid) {
    sf::Vector2i start(m_x, m_y);
    m_path = Pathfinder::FindPath(grid, start, target);
    // Skip the first node (it's the start position)
    m_pathIndex = (m_path.size() > 1) ? 1 : 0;
}

bool Robot::hasPath() const {
    return !m_path.empty() && m_pathIndex < static_cast<int>(m_path.size());
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

float Robot::getRenderX() const {
    return m_prevX + (static_cast<float>(m_x) - m_prevX) * m_lerpT;
}

float Robot::getRenderY() const {
    return m_prevY + (static_cast<float>(m_y) - m_prevY) * m_lerpT;
}

void Robot::setX(int x) { m_x = x; }
void Robot::setY(int y) { m_y = y; }
void Robot::setAutonomous(bool autonomous) { m_autonomous = autonomous; }