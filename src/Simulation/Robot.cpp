//
// Created by Devrim Yildiz on 15.01.26.
//
#include "Simulation/Robot.h"
#include "Simulation/Pathfinder.h"
#include <iostream>

Robot::Robot(int id, int startX, int startY)
    : m_id(id), m_x(startX), m_y(startY),
      m_state(RobotState::IDLE),
      m_battery(100.0f),
      m_isCarryingItem(false),
      m_currentItemId(-1),
      m_hasTask(false),
      m_pickupTarget(-1, -1),
      m_dropTarget(-1, -1),
      m_pathIndex(0),
      m_totalDistance(0.0f),
      m_tasksCompleted(0),
      m_moveTimer(0.0f)
{
    std::cout << m_id << ". Robot created at " << m_x << "," << m_y << std::endl;
}

void Robot::AssignTask(const Utils::Point& pickup, const Utils::Point& drop, int itemId) {
    m_hasTask = true;
    m_pickupTarget = pickup;
    m_dropTarget = drop;
    m_currentItemId = itemId;
    m_state = RobotState::MOVING_TO_PICKUP;
    m_currentPath.clear();
    m_pathIndex = 0;
}

void Robot::ClearTask() {
    m_hasTask = false;
    m_isCarryingItem = false;
    m_currentItemId = -1;
    m_state = RobotState::IDLE;
    m_currentPath.clear();
    m_pathIndex = 0;
}

void Robot::Update(const Grid& grid, const std::vector<Robot>& allRobots, Pathfinder& pathfinder, float deltaTime) {
    m_moveTimer += deltaTime;
    
    if (m_battery < 20.0f && m_state != RobotState::CHARGING && m_state != RobotState::LOW_BATTERY) {
        m_state = RobotState::LOW_BATTERY;
        m_hasTask = false;
        m_isCarryingItem = false;
        m_currentPath.clear();
    }
    
    ChargeBattery(grid);
    
    if (m_state == RobotState::CHARGING && m_battery >= 100.0f) {
        m_state = RobotState::IDLE;
    }
    
    if (m_state == RobotState::LOW_BATTERY) {
        FindNearestChargingStation(grid, allRobots, pathfinder);
        if (!m_currentPath.empty()) {
            m_state = RobotState::CHARGING;
        }
    }
    
    UpdateState(grid, allRobots, pathfinder);
}

void Robot::UpdateState(const Grid& grid, const std::vector<Robot>& allRobots, Pathfinder& pathfinder) {
    if (m_moveTimer < 0.2f) return;
    m_moveTimer = 0.0f;
    
    switch (m_state) {
        case RobotState::IDLE:
            break;
            
        case RobotState::MOVING_TO_PICKUP:
            if (m_currentPath.empty() || m_pathIndex >= m_currentPath.size()) {
                m_currentPath = pathfinder.FindPath(GetPosition(), m_pickupTarget, grid, allRobots, m_id);
                m_pathIndex = 0;
            }
            MoveAlongPath(grid);
            if (GetPosition() == m_pickupTarget) {
                m_state = RobotState::PICKING_UP;
            }
            break;
            
        case RobotState::PICKING_UP:
            m_isCarryingItem = true;
            m_state = RobotState::MOVING_TO_DROP;
            m_currentPath.clear();
            m_pathIndex = 0;
            break;
            
        case RobotState::MOVING_TO_DROP:
            if (m_currentPath.empty() || m_pathIndex >= m_currentPath.size()) {
                m_currentPath = pathfinder.FindPath(GetPosition(), m_dropTarget, grid, allRobots, m_id);
                m_pathIndex = 0;
            }
            MoveAlongPath(grid);
            if (GetPosition() == m_dropTarget) {
                m_state = RobotState::DROPPING;
            }
            break;
            
        case RobotState::DROPPING:
            m_isCarryingItem = false;
            m_tasksCompleted++;
            ClearTask();
            break;
            
        case RobotState::LOW_BATTERY:
        case RobotState::CHARGING:
            if (m_battery < 100.0f) {
                MoveAlongPath(grid);
            }
            break;
    }
}

void Robot::MoveAlongPath(const Grid& grid) {
    if (m_currentPath.empty() || m_pathIndex >= m_currentPath.size()) return;
    
    const Utils::Point& nextPos = m_currentPath[m_pathIndex];
    if (grid.IsWalkable(nextPos.x, nextPos.y)) {
        m_totalDistance += 1.0f;
        m_x = nextPos.x;
        m_y = nextPos.y;
        m_battery -= 0.5f;
        if (m_battery < 0.0f) m_battery = 0.0f;
        m_pathIndex++;
    } else {
        m_currentPath.clear();
        m_pathIndex = 0;
    }
}

void Robot::ChargeBattery(const Grid& grid) {
    if (grid.GetCell(m_x, m_y) == CellType::ChargingStation) {
        m_battery += 2.0f;
        if (m_battery > 100.0f) m_battery = 100.0f;
    }
}

void Robot::FindNearestChargingStation(const Grid& grid, const std::vector<Robot>& allRobots, Pathfinder& pathfinder) {
    Utils::Point nearestStation(-1, -1);
    int minDist = 999999;
    
    for (int y = 0; y < grid.GetHeight(); ++y) {
        for (int x = 0; x < grid.GetWidth(); ++x) {
            if (grid.GetCell(x, y) == CellType::ChargingStation) {
                int dist = Utils::ManhattanDistance(m_x, m_y, x, y);
                if (dist < minDist) {
                    minDist = dist;
                    nearestStation = Utils::Point(x, y);
                }
            }
        }
    }
    
    if (nearestStation.x != -1) {
        m_currentPath = pathfinder.FindPath(GetPosition(), nearestStation, grid, allRobots, m_id);
        m_pathIndex = 0;
    }
}

void Robot::TryMove(int dx, int dy, const Grid& grid) {
    int targetX = m_x + dx;
    int targetY = m_y + dy;

    if (grid.GetCell(targetX, targetY) == CellType::Wall) {
        std::cout << "Robot " << m_id << " blocked by Wall at " << targetX << "," << targetY << std::endl;
    } else {
        m_x = targetX;
        m_y = targetY;
        m_battery -= 0.5f;
        if (m_battery < 0.0f) m_battery = 0.0f;
    }
}