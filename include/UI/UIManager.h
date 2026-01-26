//
// Created by Devrim Yildiz on 26.01.26.
//

#pragma once
#include "UI/UIPanels.h"
#include <memory>
#include <vector>

class Robot;
class Grid;

// Central UI Manager for all panels
class UIManager {
public:
    UIManager();
    ~UIManager() = default;
    
    void Update(float deltaTime, const std::vector<Robot>& robots, const Grid& grid, float fps);
    void Render(sf::RenderWindow& window);
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);
    
    void ToggleUI();
    void TogglePanel(const std::string& panelName);
    
    [[nodiscard]] bool IsUIVisible() const { return m_uiVisible; }

private:
    bool m_uiVisible;
    std::unique_ptr<StatsPanel> m_statsPanel;
    std::unique_ptr<RobotPanel> m_robotPanel;
    std::unique_ptr<InventoryPanel> m_inventoryPanel;
    
    void InitializePanels();
};
