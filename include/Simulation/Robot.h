//
// Created by Devrim Yildiz on 15.01.26.
//

#pragma once
#include "Simulation/Grid.h"
#include "Simulation/TileTypes.h"
#include "Core/Utils.h"
#include <vector>

class Pathfinder;

class Robot {
public:
    Robot(int id, int startX, int startY);

    void Update(const Grid& grid, const std::vector<Robot>& allRobots, Pathfinder& pathfinder);
    void TryMove(int dx, int dy, const Grid& grid);
    
    void AssignTask(const Utils::Point& pickup, const Utils::Point& drop, int itemId);
    void ClearTask();
    bool HasTask() const { return m_hasTask; }
    bool IsIdle() const { return m_state == RobotState::IDLE && m_battery > 20.0f; }
    
    int getX() const { return m_x; }
    int getY() const { return m_y; }
    Utils::Point GetPosition() const { return Utils::Point(m_x, m_y); }
    int setX(int x) { return m_x = x; }
    int setY(int y) { return m_y = y; }
    int getId() const { return m_id; }
    
    RobotState GetState() const { return m_state; }
    float GetBattery() const { return m_battery; }
    bool IsCarryingItem() const { return m_isCarryingItem; }
    int GetCurrentItemId() const { return m_currentItemId; }
    
    float GetTotalDistance() const { return m_totalDistance; }
    int GetTasksCompleted() const { return m_tasksCompleted; }

private:
    void UpdateState(const Grid& grid, const std::vector<Robot>& allRobots, Pathfinder& pathfinder);
    void MoveAlongPath(const Grid& grid);
    void ChargeBattery(const Grid& grid);
    void FindNearestChargingStation(const Grid& grid, const std::vector<Robot>& allRobots, Pathfinder& pathfinder);

private:
    int m_id;
    int m_x;
    int m_y;
    
    RobotState m_state;
    float m_battery;
    bool m_isCarryingItem;
    int m_currentItemId;
    
    bool m_hasTask;
    Utils::Point m_pickupTarget;
    Utils::Point m_dropTarget;
    
    std::vector<Utils::Point> m_currentPath;
    int m_pathIndex;
    
    float m_totalDistance;
    int m_tasksCompleted;
    float m_moveTimer;
};
