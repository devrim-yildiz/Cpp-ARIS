//
// Created by Devrim Yildiz on 26.01.26.
//

#pragma once
#include "UI/UIPanel.h"
#include <map>
#include <string>

class Robot;
class Grid;

// Statistics Dashboard Panel
class StatsPanel : public UIPanel {
public:
    StatsPanel(const sf::Vector2f& position);
    
    void Update(float deltaTime) override;
    void Render(sf::RenderWindow& window) override;
    
    void UpdateStats(const std::vector<Robot>& robots, const Grid& grid, float fps);

private:
    float m_fps;
    int m_robotCount;
    int m_gridWidth;
    int m_gridHeight;
    int m_tasksCompleted;
    float m_totalUptime;
    
    void RenderStatLine(sf::RenderWindow& window, const std::string& label, 
                       const std::string& value, float yOffset);
};

// Robot Management Panel
class RobotPanel : public UIPanel {
public:
    RobotPanel(const sf::Vector2f& position);
    
    void Update(float deltaTime) override;
    void Render(sf::RenderWindow& window) override;
    
    void UpdateRobots(const std::vector<Robot>& robots);

private:
    struct RobotInfo {
        int id;
        int x, y;
        std::string status;
    };
    
    std::vector<RobotInfo> m_robotInfos;
    
    void RenderRobotRow(sf::RenderWindow& window, const RobotInfo& info, float yOffset);
};

// Inventory/Database Panel
class InventoryPanel : public UIPanel {
public:
    InventoryPanel(const sf::Vector2f& position);
    
    void Update(float deltaTime) override;
    void Render(sf::RenderWindow& window) override;
    
    void AddItem(const std::string& name, int quantity, const std::string& location);
    void UpdateItem(const std::string& name, int quantity);

private:
    struct InventoryItem {
        std::string name;
        int quantity;
        std::string location;
    };
    
    std::vector<InventoryItem> m_items;
    int m_scrollOffset;
    
    void RenderItemRow(sf::RenderWindow& window, const InventoryItem& item, float yOffset);
};
