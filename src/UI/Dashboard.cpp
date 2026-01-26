#include "UI/Dashboard.h"
#include "Simulation/Robot.h"
#include "Core/TaskManager.h"
#include "Simulation/Grid.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include <sstream>
#include <iomanip>

Dashboard::Dashboard()
    : m_initialized(false),
      m_frameCount(0),
      m_fps(0.0f),
      m_totalEnergyUsed(0.0f),
      m_addTaskRequested(false),
      m_spawnRobotRequested(false),
      m_killRobotRequested(false),
      m_showHeatmap(false),
      m_selectedRobotId(-1)
{
}

Dashboard::~Dashboard() {
    if (m_initialized) {
        Shutdown();
    }
}

bool Dashboard::Initialize(sf::RenderWindow& window) {
    if (!ImGui::SFML::Init(window)) {
        return false;
    }
    m_initialized = true;
    return true;
}

void Dashboard::Update(float deltaTime) {
    m_frameCount++;
    if (m_fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
        m_fps = m_frameCount / m_fpsClock.getElapsedTime().asSeconds();
        m_frameCount = 0;
        m_fpsClock.restart();
    }
}

void Dashboard::Render(
    sf::RenderWindow& window,
    const std::vector<Robot>& robots,
    const TaskManager& taskManager,
    const Grid& grid
) {
    if (!m_initialized) return;
    
    RenderStatsWindow(robots, taskManager, grid);
    RenderControlPanel();
    if (m_selectedRobotId >= 0) {
        RenderRobotInspector(robots);
    }
}

void Dashboard::RenderStatsWindow(
    const std::vector<Robot>& robots,
    const TaskManager& taskManager,
    const Grid& grid
) {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Simulation Stats");
    
    ImGui::Text("FPS: %.1f", m_fps);
    ImGui::Separator();
    
    int activeRobots = 0;
    int idleRobots = 0;
    int chargingRobots = 0;
    float totalBattery = 0.0f;
    
    for (const auto& robot : robots) {
        totalBattery += robot.GetBattery();
        if (robot.GetState() == RobotState::CHARGING || robot.GetState() == RobotState::LOW_BATTERY) {
            chargingRobots++;
        } else if (robot.GetState() == RobotState::IDLE) {
            idleRobots++;
        } else {
            activeRobots++;
        }
    }
    
    ImGui::Text("Active Robots: %d", activeRobots);
    ImGui::Text("Idle Robots: %d", idleRobots);
    ImGui::Text("Charging Robots: %d", chargingRobots);
    ImGui::Text("Total Robots: %zu", robots.size());
    
    if (!robots.empty()) {
        ImGui::Text("Avg Battery: %.1f%%", totalBattery / robots.size());
    }
    
    ImGui::Separator();
    ImGui::Text("Tasks Pending: %d", taskManager.GetPendingTaskCount());
    ImGui::Text("Tasks Completed: %d", taskManager.GetCompletedTaskCount());
    
    ImGui::Separator();
    float totalDistance = 0.0f;
    int totalTasks = 0;
    for (const auto& robot : robots) {
        totalDistance += robot.GetTotalDistance();
        totalTasks += robot.GetTasksCompleted();
    }
    ImGui::Text("Total Distance: %.1f", totalDistance);
    ImGui::Text("Total Tasks Done: %d", totalTasks);
    
    ImGui::End();
}

void Dashboard::RenderControlPanel() {
    ImGui::SetNextWindowPos(ImVec2(10, 220), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Control Panel");
    
    if (ImGui::Button("Add Random Task", ImVec2(-1, 30))) {
        m_addTaskRequested = true;
    }
    
    if (ImGui::Button("Spawn Robot", ImVec2(-1, 30))) {
        m_spawnRobotRequested = true;
    }
    
    if (ImGui::Button("Kill Random Robot", ImVec2(-1, 30))) {
        m_killRobotRequested = true;
    }
    
    ImGui::Checkbox("Show Heatmap", &m_showHeatmap);
    
    ImGui::End();
}

void Dashboard::RenderRobotInspector(const std::vector<Robot>& robots) {
    ImGui::SetNextWindowPos(ImVec2(320, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(250, 300), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Robot Inspector", nullptr, ImGuiWindowFlags_NoCollapse);
    
    const Robot* selectedRobot = nullptr;
    for (const auto& robot : robots) {
        if (robot.getId() == m_selectedRobotId) {
            selectedRobot = &robot;
            break;
        }
    }
    
    if (selectedRobot) {
        ImGui::Text("Robot ID: %d", selectedRobot->getId());
        ImGui::Separator();
        
        ImGui::Text("Position: (%d, %d)", selectedRobot->getX(), selectedRobot->getY());
        
        const char* stateNames[] = {
            "IDLE", "MOVING_TO_PICKUP", "PICKING_UP",
            "MOVING_TO_DROP", "DROPPING", "LOW_BATTERY", "CHARGING"
        };
        int stateIndex = static_cast<int>(selectedRobot->GetState());
        ImGui::Text("State: %s", stateNames[stateIndex]);
        
        ImGui::ProgressBar(selectedRobot->GetBattery() / 100.0f, ImVec2(-1, 0), 
                          (std::to_string(static_cast<int>(selectedRobot->GetBattery())) + "%").c_str());
        
        ImGui::Text("Carrying Item: %s", selectedRobot->IsCarryingItem() ? "Yes" : "No");
        if (selectedRobot->IsCarryingItem()) {
            ImGui::Text("Item ID: %d", selectedRobot->GetCurrentItemId());
        }
        
        ImGui::Separator();
        ImGui::Text("Total Distance: %.1f", selectedRobot->GetTotalDistance());
        ImGui::Text("Tasks Completed: %d", selectedRobot->GetTasksCompleted());
        
        if (ImGui::Button("Deselect", ImVec2(-1, 0))) {
            m_selectedRobotId = -1;
        }
    } else {
        ImGui::Text("No robot selected");
        ImGui::Text("Click on a robot to inspect");
        if (ImGui::Button("Close", ImVec2(-1, 0))) {
            m_selectedRobotId = -1;
        }
    }
    
    ImGui::End();
}

void Dashboard::ProcessEvent(const sf::Event& event) {
    if (m_initialized) {
        ImGui::SFML::ProcessEvent(event);
    }
}

void Dashboard::Shutdown() {
    if (m_initialized) {
        ImGui::SFML::Shutdown();
        m_initialized = false;
    }
}
