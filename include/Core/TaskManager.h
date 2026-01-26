#pragma once
#include <queue>
#include <vector>
#include "Core/Utils.h"

class Robot;
class Grid;

struct Task {
    int itemId;
    Utils::Point pickupCell;
    Utils::Point dropCell;
    int priority;
    int assignedRobotId;
    
    Task(int id, const Utils::Point& pickup, const Utils::Point& drop, int prio = 1)
        : itemId(id), pickupCell(pickup), dropCell(drop), priority(prio), assignedRobotId(-1) {}
    
    bool operator<(const Task& other) const {
        return priority < other.priority;
    }
};

class TaskManager {
public:
    TaskManager();
    
    void AddTask(const Task& task);
    void Update(std::vector<Robot>& robots, const Grid& grid);
    
    int GetPendingTaskCount() const { return static_cast<int>(m_tasks.size()); }
    int GetCompletedTaskCount() const { return m_completedTasks; }
    
    void GenerateRandomTask(const Grid& grid);

private:
    void AssignTaskToNearestRobot(const Task& task, std::vector<Robot>& robots, const Grid& grid);
    Robot* FindNearestIdleRobot(const Utils::Point& position, std::vector<Robot>& robots);

private:
    std::priority_queue<Task> m_tasks;
    int m_completedTasks;
    int m_nextItemId;
};
