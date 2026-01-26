# Actionable Improvements Guide

This document provides specific, actionable recommendations to improve the ARIS codebase based on the code quality assessment.

---

## Quick Fixes (< 1 hour)

### 1. Fix Typo in Grid.cpp
**File:** `src/Simulation/Grid.cpp:17`  
**Current:**
```cpp
// bounds checkkk
```
**Fix:**
```cpp
// Bounds check
```

### 2. Remove Dead Code
**File:** `include/Simulation/Robot.h:25-26`  
**Current:**
```cpp
// float m_batteryLevel;
// bool m_isCarryingItem;
```
**Fix:** Delete these commented lines entirely.

### 3. Remove Unused Function
**File:** `src/main.cpp:15-22`  
**Current:**
```cpp
void modifyCurrentWorkingDirectory()
{
    while (!std::filesystem::exists("resources"))
    {
        std::filesystem::current_path(std::filesystem::current_path().parent_path());
    }
    auto cwd = std::filesystem::current_path();
}
```
**Fix:** Either use it or delete it. If needed, call it before `app.Run()` in main().

### 4. Fix Setter Return Types
**File:** `include/Simulation/Robot.h:16-17` and `src/Simulation/Robot.cpp:39-44`  
**Current:**
```cpp
int setX(int x);
int setY(int y);
```
**Fix:**
```cpp
void setX(int x);
void setY(int y);
```

---

## Error Handling Improvements (2-4 hours)

### 5. Use std::optional for Grid::GetCell
**File:** `include/Simulation/Grid.h` and `src/Simulation/Grid.cpp`

**Current:**
```cpp
[[nodiscard]] CellType GetCell(int x, int y) const;

// In Grid.cpp
CellType Grid::GetCell(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return CellType::Wall;  // Silent failure
    }
    int index = GetIndex(x, y);
    return m_cells[index];
}
```

**Improved:**
```cpp
// Grid.h
#include <optional>

[[nodiscard]] std::optional<CellType> GetCell(int x, int y) const;

// Grid.cpp
std::optional<CellType> Grid::GetCell(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return std::nullopt;  // Explicit failure
    }
    int index = GetIndex(x, y);
    return m_cells[index];
}
```

**Update callers:**
```cpp
// Robot.cpp
void Robot::TryMove(int dx, int dy, const Grid& grid) {
    int targetX = m_x + dx;
    int targetY = m_y + dy;
    
    auto cell = grid.GetCell(targetX, targetY);
    if (!cell.has_value()) {
        std::cout << "Robot " << m_id << " tried to move out of bounds!" << std::endl;
        return;
    }
    
    if (cell.value() == CellType::Wall) {
        std::cout << "Robot " << m_id << " blocked by Wall at " 
                  << targetX << "," << targetY << std::endl;
    } else {
        m_x = targetX;
        m_y = targetY;
    }
}
```

### 6. Add Input Validation to Robot Constructor
**File:** `src/Simulation/Robot.cpp`

**Current:**
```cpp
Robot::Robot(int id, int startX, int startY)
    : m_id(id), m_x(startX), m_y(startY)
{
    std::cout << m_id << ". Robot created at " << m_x << "," << m_y << std::endl;
}
```

**Improved:**
```cpp
Robot::Robot(int id, int startX, int startY)
    : m_id(id), m_x(startX), m_y(startY)
{
    if (id < 0) {
        throw std::invalid_argument("Robot ID must be non-negative");
    }
    std::cout << m_id << ". Robot created at " << m_x << "," << m_y << std::endl;
}
```

**Or without exceptions:**
```cpp
Robot::Robot(int id, int startX, int startY, const Grid& grid)
    : m_id(id), m_x(startX), m_y(startY)
{
    if (id < 0) {
        std::cerr << "Warning: Robot ID must be non-negative, using absolute value" << std::endl;
        m_id = std::abs(id);
    }
    
    // Validate position is within grid
    auto cell = grid.GetCell(startX, startY);
    if (!cell.has_value() || cell.value() == CellType::Wall) {
        std::cerr << "Warning: Invalid starting position, moving to (0,0)" << std::endl;
        m_x = 0;
        m_y = 0;
    }
    
    std::cout << m_id << ". Robot created at " << m_x << "," << m_y << std::endl;
}
```

### 7. Add Bounds Checking to Grid Vector Access
**File:** `src/Simulation/Grid.cpp`

**Current:**
```cpp
CellType Grid::GetCell(int x, int y) const {
    // bounds check
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return CellType::Wall;
    }
    
    int index = GetIndex(x, y);
    return m_cells[index];  // Dangerous if GetIndex returns -1
}
```

**Improved:**
```cpp
std::optional<CellType> Grid::GetCell(int x, int y) const {
    int index = GetIndex(x, y);
    if (index < 0 || index >= static_cast<int>(m_cells.size())) {
        return std::nullopt;
    }
    return m_cells[index];
}
```

---

## Add Testing Infrastructure (4-6 hours)

### 8. Set Up Google Test

**Step 1:** Update `CMakeLists.txt`:
```cmake
# Add Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Add test executable
add_executable(
    tests
    tests/GridTests.cpp
    tests/RobotTests.cpp
    src/Simulation/Grid.cpp
    src/Simulation/Robot.cpp
)

target_include_directories(
    tests
    PRIVATE ${PROJECT_SOURCE_DIR}/include/
)

target_link_libraries(
    tests
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)
```

**Step 2:** Create `tests/GridTests.cpp`:
```cpp
#include <gtest/gtest.h>
#include "Simulation/Grid.h"

TEST(GridTest, ConstructorInitialization) {
    Grid grid(10, 10);
    EXPECT_EQ(grid.GetWidth(), 10);
    EXPECT_EQ(grid.GetHeight(), 10);
}

TEST(GridTest, DefaultCellTypeIsEmpty) {
    Grid grid(5, 5);
    auto cell = grid.GetCell(0, 0);
    ASSERT_TRUE(cell.has_value());
    EXPECT_EQ(cell.value(), CellType::Empty);
}

TEST(GridTest, SetAndGetCell) {
    Grid grid(5, 5);
    grid.SetCell(2, 3, CellType::Wall);
    auto cell = grid.GetCell(2, 3);
    ASSERT_TRUE(cell.has_value());
    EXPECT_EQ(cell.value(), CellType::Wall);
}

TEST(GridTest, OutOfBoundsReturnsNullopt) {
    Grid grid(5, 5);
    
    EXPECT_FALSE(grid.GetCell(-1, 0).has_value());
    EXPECT_FALSE(grid.GetCell(0, -1).has_value());
    EXPECT_FALSE(grid.GetCell(5, 0).has_value());
    EXPECT_FALSE(grid.GetCell(0, 5).has_value());
    EXPECT_FALSE(grid.GetCell(100, 100).has_value());
}

TEST(GridTest, SetCellOutOfBoundsIsNoOp) {
    Grid grid(5, 5);
    
    // These should not crash
    grid.SetCell(-1, 0, CellType::Wall);
    grid.SetCell(0, -1, CellType::Wall);
    grid.SetCell(5, 0, CellType::Wall);
    grid.SetCell(0, 5, CellType::Wall);
    
    // Verify grid is unchanged
    auto cell = grid.GetCell(0, 0);
    ASSERT_TRUE(cell.has_value());
    EXPECT_EQ(cell.value(), CellType::Empty);
}
```

**Step 3:** Create `tests/RobotTests.cpp`:
```cpp
#include <gtest/gtest.h>
#include "Simulation/Robot.h"
#include "Simulation/Grid.h"

TEST(RobotTest, ConstructorInitialization) {
    Robot robot(1, 5, 10);
    EXPECT_EQ(robot.getId(), 1);
    EXPECT_EQ(robot.getX(), 5);
    EXPECT_EQ(robot.getY(), 10);
}

TEST(RobotTest, MoveInEmptyCell) {
    Grid grid(10, 10);
    Robot robot(1, 5, 5);
    
    robot.TryMove(1, 0, grid);  // Move right
    EXPECT_EQ(robot.getX(), 6);
    EXPECT_EQ(robot.getY(), 5);
}

TEST(RobotTest, MoveBlockedByWall) {
    Grid grid(10, 10);
    grid.SetCell(6, 5, CellType::Wall);
    Robot robot(1, 5, 5);
    
    robot.TryMove(1, 0, grid);  // Try to move into wall
    EXPECT_EQ(robot.getX(), 5);  // Should not move
    EXPECT_EQ(robot.getY(), 5);
}

TEST(RobotTest, MoveOutOfBounds) {
    Grid grid(10, 10);
    Robot robot(1, 0, 0);
    
    robot.TryMove(-1, 0, grid);  // Try to move left from edge
    EXPECT_EQ(robot.getX(), 0);  // Should not move
    EXPECT_EQ(robot.getY(), 0);
}

TEST(RobotTest, MultipleMovements) {
    Grid grid(10, 10);
    Robot robot(1, 5, 5);
    
    robot.TryMove(0, 1, grid);   // Down
    robot.TryMove(1, 0, grid);   // Right
    robot.TryMove(0, -1, grid);  // Up
    robot.TryMove(-1, 0, grid);  // Left
    
    EXPECT_EQ(robot.getX(), 5);  // Should be back at start
    EXPECT_EQ(robot.getY(), 5);
}
```

**Step 4:** Update CI to run tests:
```yaml
# .github/workflows/build-release.yml
# Add after Build step:
      - name: Test
        working-directory: ${{github.workspace}}/build
        shell: bash
        run: ctest --output-on-failure
```

---

## Code Modernization (6-8 hours)

### 9. Use Structured Bindings
**File:** `src/Renderer/WorldRenderer.cpp`

**Current:**
```cpp
for (int y = 0; y < grid.GetHeight(); ++y) {
    for (int x = 0; x < grid.GetWidth(); ++x) {
```

**Improved:**
```cpp
// Create a helper range function
struct GridRange {
    int width, height;
    
    auto cells() const {
        return std::views::cartesian_product(
            std::views::iota(0, height),
            std::views::iota(0, width)
        );
    }
};

// Usage (C++23) or keep current for C++17
```

### 10. Add Configuration System
**File:** `resources/config.json` (new)

```json
{
  "window": {
    "width": 800,
    "height": 600,
    "title": "ARIS Simulation",
    "framerate": 60
  },
  "grid": {
    "width": 20,
    "height": 15,
    "cellSize": 32
  },
  "simulation": {
    "numRobots": 2,
    "robotSpeed": 1.0
  }
}
```

**File:** `include/Core/Config.h` (new)
```cpp
#pragma once
#include <string>

struct Config {
    struct Window {
        int width;
        int height;
        std::string title;
        int framerate;
    } window;
    
    struct Grid {
        int width;
        int height;
        float cellSize;
    } grid;
    
    struct Simulation {
        int numRobots;
        float robotSpeed;
    } simulation;
    
    static Config LoadFromFile(const std::string& path);
};
```

### 11. Add Logging Framework
Instead of `std::cout`, use a proper logger:

**File:** `include/Core/Logger.h` (new)
```cpp
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class Logger {
public:
    static Logger& Get() {
        static Logger instance;
        return instance;
    }
    
    void SetLogLevel(LogLevel level) { m_minLevel = level; }
    
    void Debug(const std::string& msg) { Log(LogLevel::Debug, msg); }
    void Info(const std::string& msg) { Log(LogLevel::Info, msg); }
    void Warning(const std::string& msg) { Log(LogLevel::Warning, msg); }
    void Error(const std::string& msg) { Log(LogLevel::Error, msg); }
    
private:
    Logger() = default;
    LogLevel m_minLevel = LogLevel::Info;
    
    void Log(LogLevel level, const std::string& msg) {
        if (level < m_minLevel) return;
        
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::cout << "[" << LevelToString(level) << "] "
                  << std::put_time(std::localtime(&time), "%H:%M:%S") << " "
                  << msg << std::endl;
    }
    
    const char* LevelToString(LogLevel level) {
        switch(level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            default: return "UNKNOWN";
        }
    }
};

// Convenience macros
#define LOG_DEBUG(msg) Logger::Get().Debug(msg)
#define LOG_INFO(msg) Logger::Get().Info(msg)
#define LOG_WARNING(msg) Logger::Get().Warning(msg)
#define LOG_ERROR(msg) Logger::Get().Error(msg)
```

**Usage:**
```cpp
// Replace std::cout with:
LOG_INFO("System Initialized. Robots active: " + std::to_string(m_robots.size()));
```

---

## Documentation Improvements (2-3 hours)

### 12. Add Doxygen Comments

**File:** `include/Simulation/Grid.h`
```cpp
/**
 * @brief Represents a 2D grid for the warehouse simulation
 * 
 * The Grid stores cells in a flat vector for cache efficiency.
 * Coordinates are 0-indexed with (0,0) at top-left.
 */
class Grid {
public:
    /**
     * @brief Constructs a grid with specified dimensions
     * @param width Number of cells horizontally
     * @param height Number of cells vertically
     * @throws std::invalid_argument if width or height <= 0
     */
    Grid(int width, int height);
    
    /**
     * @brief Gets the cell type at specified coordinates
     * @param x Horizontal coordinate (0 to width-1)
     * @param y Vertical coordinate (0 to height-1)
     * @return Cell type if in bounds, std::nullopt otherwise
     */
    [[nodiscard]] std::optional<CellType> GetCell(int x, int y) const;
    
    // ... rest of class
};
```

### 13. Add CONTRIBUTING.md
Create a guide for contributors explaining:
- Code style guidelines
- How to build and test
- How to submit PRs
- Development workflow

---

## Performance Optimizations (Optional, 4-6 hours)

### 14. Optimize Rendering with Dirty Rectangles
Instead of redrawing the entire grid every frame, only redraw changed areas.

### 15. Add Spatial Indexing
For larger simulations, use a quadtree or grid-based spatial index for collision detection.

---

## Priority Order

**Week 1:** Quick Fixes (Items 1-4)  
**Week 2:** Error Handling (Items 5-7)  
**Week 3:** Testing (Item 8)  
**Week 4:** Documentation (Items 12-13)  
**Week 5+:** Modernization & Features (Items 9-11, 14-15)

---

## Conclusion

Start with the quick fixes and error handling to establish a more robust foundation. Then add testing before implementing new features. This approach will make the codebase more maintainable and prevent technical debt accumulation.

**Remember:** Good code is code that works correctly, is easy to understand, and easy to modify. These improvements move the project in that direction.
