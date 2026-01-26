//
// Created by Devrim Yildiz on 26.01.26.
//

#include "UI/UIManager.h"
#include "Simulation/Robot.h"
#include "Simulation/Grid.h"

UIManager::UIManager()
    : m_uiVisible(true)
{
    InitializePanels();
}

void UIManager::InitializePanels() {
    // Create panels with smart positioning
    m_statsPanel = std::make_unique<StatsPanel>(sf::Vector2f(10.f, 10.f));
    m_robotPanel = std::make_unique<RobotPanel>(sf::Vector2f(10.f, 270.f));
    m_inventoryPanel = std::make_unique<InventoryPanel>(sf::Vector2f(450.f, 10.f));
}

void UIManager::Update(float deltaTime, const std::vector<Robot>& robots, const Grid& grid, float fps) {
    if (!m_uiVisible) return;
    
    // Update all panels
    m_statsPanel->Update(deltaTime);
    m_statsPanel->UpdateStats(robots, grid, fps);
    
    m_robotPanel->Update(deltaTime);
    m_robotPanel->UpdateRobots(robots);
    
    m_inventoryPanel->Update(deltaTime);
}

void UIManager::Render(sf::RenderWindow& window) {
    if (!m_uiVisible) return;
    
    // Render all panels
    m_statsPanel->Render(window);
    m_robotPanel->Render(window);
    m_inventoryPanel->Render(window);
}

void UIManager::HandleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (!m_uiVisible) return;
    
    // Pass events to all panels
    m_inventoryPanel->HandleEvent(event, window);
    m_robotPanel->HandleEvent(event, window);
    m_statsPanel->HandleEvent(event, window);
}

void UIManager::ToggleUI() {
    m_uiVisible = !m_uiVisible;
}

void UIManager::TogglePanel(const std::string& panelName) {
    if (panelName == "stats") {
        m_statsPanel->SetVisible(!m_statsPanel->IsVisible());
    }
    else if (panelName == "robots") {
        m_robotPanel->SetVisible(!m_robotPanel->IsVisible());
    }
    else if (panelName == "inventory") {
        m_inventoryPanel->SetVisible(!m_inventoryPanel->IsVisible());
    }
}
