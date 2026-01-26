#pragma once
#include <SFML/Graphics.hpp>
#include "imgui.h"

class Application;
class Robot;
class TaskManager;
class Grid;

class Dashboard {
public:
    Dashboard();
    ~Dashboard();
    
    bool Initialize(sf::RenderWindow& window);
    void Update(float deltaTime);
    void Render(
        sf::RenderWindow& window,
        const std::vector<Robot>& robots,
        const TaskManager& taskManager,
        const Grid& grid
    );
    void ProcessEvent(const sf::Event& event);
    void Shutdown();
    
    bool ShouldAddTask() const { return m_addTaskRequested; }
    bool ShouldSpawnRobot() const { return m_spawnRobotRequested; }
    bool ShouldKillRobot() const { return m_killRobotRequested; }
    void ResetRequests() {
        m_addTaskRequested = false;
        m_spawnRobotRequested = false;
        m_killRobotRequested = false;
    }
    
    int GetSelectedRobotId() const { return m_selectedRobotId; }
    void SetSelectedRobotId(int id) { m_selectedRobotId = id; }
    
    bool IsHeatmapEnabled() const { return m_showHeatmap; }

private:
    void RenderStatsWindow(
        const std::vector<Robot>& robots,
        const TaskManager& taskManager,
        const Grid& grid
    );
    void RenderControlPanel();
    void RenderRobotInspector(const std::vector<Robot>& robots);

private:
    bool m_initialized;
    sf::Clock m_fpsClock;
    int m_frameCount;
    float m_fps;
    float m_totalEnergyUsed;
    
    bool m_addTaskRequested;
    bool m_spawnRobotRequested;
    bool m_killRobotRequested;
    bool m_showHeatmap;
    int m_selectedRobotId;
};
