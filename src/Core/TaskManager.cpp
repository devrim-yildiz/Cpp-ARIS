#include "Core/TaskManager.h"
#include "Simulation/Robot.h"
#include "Simulation/Grid.h"
#include <iostream>
#include <random>
#include <limits>

namespace {
    constexpr int INFINITE_DISTANCE = std::numeric_limits<int>::max();
}

TaskManager::TaskManager()
    : m_completedTasks(0), m_nextItemId(1000)
{
}

void TaskManager::AddTask(const Task& task) {
    m_tasks.push(task);
    std::cout << "Task added: Item " << task.itemId << " from (" 
              << task.pickupCell.x << "," << task.pickupCell.y << ") to ("
              << task.dropCell.x << "," << task.dropCell.y << ")" << std::endl;
}

void TaskManager::Update(std::vector<Robot>& robots, const Grid& grid) {
    while (!m_tasks.empty()) {
        Task task = m_tasks.top();
        m_tasks.pop();
        AssignTaskToNearestRobot(task, robots, grid);
    }
    
    for (auto& robot : robots) {
        if (!robot.HasTask() && robot.GetState() == RobotState::IDLE) {
        }
    }
}

void TaskManager::AssignTaskToNearestRobot(const Task& task, std::vector<Robot>& robots, const Grid& grid) {
    Robot* nearestRobot = FindNearestIdleRobot(task.pickupCell, robots);
    
    if (nearestRobot) {
        nearestRobot->AssignTask(task.pickupCell, task.dropCell, task.itemId);
        std::cout << "Task " << task.itemId << " assigned to Robot " << nearestRobot->getId() << std::endl;
    } else {
        m_tasks.push(task);
    }
}

Robot* TaskManager::FindNearestIdleRobot(const Utils::Point& position, std::vector<Robot>& robots) {
    Robot* nearest = nullptr;
    int minDist = INFINITE_DISTANCE;
    
    for (auto& robot : robots) {
        if (robot.IsIdle()) {
            int dist = Utils::ManhattanDistance(robot.GetPosition(), position);
            if (dist < minDist) {
                minDist = dist;
                nearest = &robot;
            }
        }
    }
    
    return nearest;
}

void TaskManager::GenerateRandomTask(const Grid& grid) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    std::vector<Utils::Point> validCells;
    for (int y = 0; y < grid.GetHeight(); ++y) {
        for (int x = 0; x < grid.GetWidth(); ++x) {
            if (grid.IsWalkable(x, y)) {
                validCells.push_back(Utils::Point(x, y));
            }
        }
    }
    
    if (validCells.size() < 2) return;
    
    std::uniform_int_distribution<> dis(0, validCells.size() - 1);
    Utils::Point pickup = validCells[dis(gen)];
    Utils::Point drop = validCells[dis(gen)];
    
    while (pickup == drop && validCells.size() > 1) {
        drop = validCells[dis(gen)];
    }
    
    AddTask(Task(m_nextItemId++, pickup, drop, 1));
}
