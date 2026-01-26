//
// Created by Devrim Yildiz on 26.01.26.
//

#include "UI/UIPanels.h"
#include "Simulation/Robot.h"
#include "Simulation/Grid.h"
#include <sstream>
#include <iomanip>

// ============================================================================
// StatsPanel Implementation
// ============================================================================

StatsPanel::StatsPanel(const sf::Vector2f& position)
    : UIPanel("System Statistics", position, sf::Vector2f(300.f, 250.f))
    , m_fps(0.f)
    , m_robotCount(0)
    , m_gridWidth(0)
    , m_gridHeight(0)
    , m_tasksCompleted(0)
    , m_totalUptime(0.f)
{
}

void StatsPanel::Update(float deltaTime) {
    UIPanel::Update(deltaTime);
    m_totalUptime += deltaTime;
}

void StatsPanel::UpdateStats(const std::vector<Robot>& robots, const Grid& grid, float fps) {
    m_fps = fps;
    m_robotCount = static_cast<int>(robots.size());
    m_gridWidth = grid.GetWidth();
    m_gridHeight = grid.GetHeight();
}

void StatsPanel::RenderStatLine(sf::RenderWindow& window, const std::string& label, 
                                const std::string& value, float yOffset) {
    sf::Text labelText, valueText;
    labelText.setFont(m_font);
    valueText.setFont(m_font);
    
    labelText.setString(label);
    labelText.setCharacterSize(14);
    labelText.setFillColor(sf::Color(180, 180, 185, 255));
    labelText.setPosition(m_position.x + 15.f, m_position.y + yOffset);
    
    valueText.setString(value);
    valueText.setCharacterSize(14);
    valueText.setFillColor(sf::Color(100, 200, 255, 255));
    valueText.setPosition(m_position.x + 180.f, m_position.y + yOffset);
    
    window.draw(labelText);
    window.draw(valueText);
}

void StatsPanel::Render(sf::RenderWindow& window) {
    UIPanel::Render(window);
    if (!m_visible) return;
    
    float yOffset = 45.f;
    float lineSpacing = 25.f;
    
    // FPS
    std::ostringstream fpsStream;
    fpsStream << std::fixed << std::setprecision(1) << m_fps;
    RenderStatLine(window, "FPS:", fpsStream.str(), yOffset);
    yOffset += lineSpacing;
    
    // Grid Size
    std::ostringstream gridStream;
    gridStream << m_gridWidth << " x " << m_gridHeight;
    RenderStatLine(window, "Grid Size:", gridStream.str(), yOffset);
    yOffset += lineSpacing;
    
    // Active Robots
    RenderStatLine(window, "Active Robots:", std::to_string(m_robotCount), yOffset);
    yOffset += lineSpacing;
    
    // Tasks Completed
    RenderStatLine(window, "Tasks Done:", std::to_string(m_tasksCompleted), yOffset);
    yOffset += lineSpacing;
    
    // Uptime
    int minutes = static_cast<int>(m_totalUptime) / 60;
    int seconds = static_cast<int>(m_totalUptime) % 60;
    std::ostringstream uptimeStream;
    uptimeStream << minutes << "m " << seconds << "s";
    RenderStatLine(window, "Uptime:", uptimeStream.str(), yOffset);
    yOffset += lineSpacing;
    
    // Status
    RenderStatLine(window, "Status:", "OPERATIONAL", yOffset);
}

// ============================================================================
// RobotPanel Implementation
// ============================================================================

RobotPanel::RobotPanel(const sf::Vector2f& position)
    : UIPanel("Robot Fleet", position, sf::Vector2f(350.f, 300.f))
{
}

void RobotPanel::Update(float deltaTime) {
    UIPanel::Update(deltaTime);
}

void RobotPanel::UpdateRobots(const std::vector<Robot>& robots) {
    m_robotInfos.clear();
    for (const auto& robot : robots) {
        RobotInfo info;
        info.id = robot.getId();
        info.x = robot.getX();
        info.y = robot.getY();
        info.status = "Active";
        m_robotInfos.push_back(info);
    }
}

void RobotPanel::RenderRobotRow(sf::RenderWindow& window, const RobotInfo& info, float yOffset) {
    std::ostringstream ss;
    ss << "Robot #" << info.id << "  |  Pos: (" << info.x << ", " << info.y << ")  |  " << info.status;
    
    sf::Text text;
    text.setFont(m_font);
    text.setString(ss.str());
    text.setCharacterSize(13);
    text.setFillColor(m_textColor);
    text.setPosition(m_position.x + 15.f, m_position.y + yOffset);
    
    window.draw(text);
}

void RobotPanel::Render(sf::RenderWindow& window) {
    UIPanel::Render(window);
    if (!m_visible) return;
    
    float yOffset = 45.f;
    float lineSpacing = 25.f;
    
    // Header
    sf::Text headerText;
    headerText.setFont(m_font);
    headerText.setString("ID      Position        Status");
    headerText.setCharacterSize(13);
    headerText.setFillColor(sf::Color(150, 150, 155, 255));
    headerText.setPosition(m_position.x + 15.f, m_position.y + yOffset);
    window.draw(headerText);
    
    yOffset += lineSpacing + 5.f;
    
    // Render each robot
    for (const auto& info : m_robotInfos) {
        RenderRobotRow(window, info, yOffset);
        yOffset += lineSpacing;
    }
    
    if (m_robotInfos.empty()) {
        sf::Text emptyText;
        emptyText.setFont(m_font);
        emptyText.setString("No robots active");
        emptyText.setCharacterSize(13);
        emptyText.setFillColor(sf::Color(150, 150, 155, 255));
        emptyText.setPosition(m_position.x + 15.f, m_position.y + yOffset);
        window.draw(emptyText);
    }
}

// ============================================================================
// InventoryPanel Implementation
// ============================================================================

InventoryPanel::InventoryPanel(const sf::Vector2f& position)
    : UIPanel("Inventory Database", position, sf::Vector2f(400.f, 350.f))
    , m_scrollOffset(0)
{
    // Add some sample inventory items
    AddItem("Widget-A", 150, "A-12");
    AddItem("Gadget-B", 89, "B-05");
    AddItem("Component-C", 234, "C-18");
    AddItem("Part-D", 67, "D-09");
    AddItem("Module-E", 112, "E-22");
}

void InventoryPanel::Update(float deltaTime) {
    UIPanel::Update(deltaTime);
}

void InventoryPanel::AddItem(const std::string& name, int quantity, const std::string& location) {
    InventoryItem item;
    item.name = name;
    item.quantity = quantity;
    item.location = location;
    m_items.push_back(item);
}

void InventoryPanel::UpdateItem(const std::string& name, int quantity) {
    for (auto& item : m_items) {
        if (item.name == name) {
            item.quantity = quantity;
            break;
        }
    }
}

void InventoryPanel::RenderItemRow(sf::RenderWindow& window, const InventoryItem& item, float yOffset) {
    // Item name
    sf::Text nameText;
    nameText.setFont(m_font);
    nameText.setString(item.name);
    nameText.setCharacterSize(13);
    nameText.setFillColor(m_textColor);
    nameText.setPosition(m_position.x + 15.f, m_position.y + yOffset);
    window.draw(nameText);
    
    // Quantity
    sf::Text qtyText;
    qtyText.setFont(m_font);
    qtyText.setString(std::to_string(item.quantity));
    qtyText.setCharacterSize(13);
    qtyText.setFillColor(sf::Color(100, 255, 150, 255));
    qtyText.setPosition(m_position.x + 200.f, m_position.y + yOffset);
    window.draw(qtyText);
    
    // Location
    sf::Text locText;
    locText.setFont(m_font);
    locText.setString(item.location);
    locText.setCharacterSize(13);
    locText.setFillColor(sf::Color(255, 200, 100, 255));
    locText.setPosition(m_position.x + 300.f, m_position.y + yOffset);
    window.draw(locText);
}

void InventoryPanel::Render(sf::RenderWindow& window) {
    UIPanel::Render(window);
    if (!m_visible) return;
    
    float yOffset = 45.f;
    float lineSpacing = 25.f;
    
    // Header
    sf::Text headerText;
    headerText.setFont(m_font);
    headerText.setString("Item Name          Qty     Location");
    headerText.setCharacterSize(13);
    headerText.setFillColor(sf::Color(150, 150, 155, 255));
    headerText.setPosition(m_position.x + 15.f, m_position.y + yOffset);
    window.draw(headerText);
    
    yOffset += lineSpacing + 5.f;
    
    // Render items
    for (const auto& item : m_items) {
        RenderItemRow(window, item, yOffset);
        yOffset += lineSpacing;
        
        // Don't render beyond panel bounds
        if (yOffset > m_position.y + m_size.y - 40.f) break;
    }
}
