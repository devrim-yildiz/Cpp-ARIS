# A.R.I.S. — Complete Project Overview for AI Assistants

> **Purpose of this document:** This file provides a complete, self-contained overview of the
> A.R.I.S. (Automated Robot Inventory System) C++ project so that any AI assistant can understand
> the full codebase without needing to see the actual files.

---

## 1. What Is This Project?

A.R.I.S. is a **real-time 2D warehouse simulation** written in **C++17**. It renders an
interactive grid-based warehouse where autonomous robots can move around, avoid walls, and
(eventually) carry out inventory tasks. The project uses **SFML 2.6** for rendering and window
management.

**Current status:** Early-stage / proof-of-concept. The core architecture (game loop, grid, robot
movement, rendering) is working. Advanced features like pathfinding, database, and UI overlay are
**not yet implemented**.

---

## 2. Directory Structure

```
Cpp-ARIS/
├── CMakeLists.txt                        # Build configuration (CMake 3.5+)
├── README.md                             # Project README (in German)
├── LICENSE.md                            # MIT License
├── .gitmodules                           # SFML submodule reference
│
├── include/                              # All header files
│   ├── Core/
│   │   └── Application.h                # Main application class
│   ├── Renderer/
│   │   └── WorldRenderer.h              # SFML drawing logic
│   └── Simulation/
│       ├── Grid.h                        # Grid system + CellType enum
│       └── Robot.h                       # Robot agent class
│
├── src/                                  # All implementation files
│   ├── main.cpp                          # Entry point
│   ├── Core/
│   │   └── Application.cpp              # Game loop implementation
│   ├── Renderer/
│   │   └── WorldRenderer.cpp            # Rendering implementation
│   └── Simulation/
│       ├── Grid.cpp                      # Grid logic
│       └── Robot.cpp                     # Robot movement logic
│
├── resources/
│   └── FiraCode-Regular.ttf             # Font file (not used yet)
│
└── vendors/
    └── sfml/                             # SFML 2.6 (Git submodule)
```

**Total:** 9 source files (5 `.cpp`, 4 `.h`), ~280 lines of code.

---

## 3. Build System

- **Tool:** CMake (minimum version 3.5.0)
- **C++ Standard:** C++17 (set per-target, not globally, because SFML audio breaks with global C++17)
- **SFML:** Built from source as a subdirectory (`vendors/sfml/`)
- **Linked libraries:** `sfml-graphics`, `sfml-window`, `sfml-system`
- **Output binary:** `build/bin/A.R.I.S`
- **Resources:** The `resources/` folder is copied into `build/bin/` at configure time

### How to build

```bash
git clone --recurse-submodules https://github.com/devrim-yildiz/Cpp-ARIS.git
cd Cpp-ARIS
mkdir build && cd build
cmake ..
make        # or: cmake --build .
./bin/A.R.I.S
```

### CMakeLists.txt Details

```cmake
cmake_minimum_required(VERSION 3.5.0)
project(A.R.I.S VERSION 0.1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/)

add_subdirectory(vendors/sfml/)                           # Build SFML from source

file(GLOB_RECURSE ALL_SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE ALL_HEADER_FILES "${CMAKE_SOURCE_DIR}/include/*.hpp")

add_executable(${PROJECT_NAME} ${ALL_SOURCE_FILES} ${ALL_HEADER_FILES}
    include/Simulation/Robot.h
    src/Simulation/Robot.cpp
    src/Simulation/Grid.cpp
    src/Renderer/WorldRenderer.cpp
    include/Renderer/WorldRenderer.h
    include/Core/Application.h
    src/Core/Application.cpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/include/
    PRIVATE ${PROJECT_SOURCE_DIR}/vendors/sfml/include/
)

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR}/bin)
```

> **Note:** The GLOB_RECURSE for headers looks for `*.hpp` files, but all headers are `.h` files.
> This means the glob finds nothing — the headers are added individually in the
> `add_executable(...)` call instead. This is not a bug per se, but worth noting.

---

## 4. Architecture

The project follows a classic **MVC / Game-Loop** pattern:

```
┌──────────────────────────────────────────────┐
│                  main.cpp                     │
│         Creates Application, calls Run()      │
└──────────────────┬───────────────────────────┘
                   │
                   ▼
┌──────────────────────────────────────────────┐
│              Application (Core)               │
│                                               │
│  Run() loop:                                  │
│    1. ProcessEvents()  ← keyboard input       │
│    2. Update()         ← game logic (empty)   │
│    3. Render()         ← draw everything      │
│                                               │
│  Owns: m_window, m_grid, m_renderer, m_robots │
└──────┬──────────────┬────────────┬───────────┘
       │              │            │
       ▼              ▼            ▼
┌──────────┐  ┌───────────┐  ┌──────────────┐
│   Grid   │  │  Robot(s)  │  │WorldRenderer │
│(Simulation│  │(Simulation)│  │ (Renderer)   │
│  layer)  │  │            │  │              │
└──────────┘  └────────────┘  └──────────────┘
```

**Data flow each frame:**
1. `ProcessEvents()` — reads keyboard input, moves Robot 0 via arrow keys
2. `Update()` — currently empty; placeholder for future autonomous robot logic
3. `Render()` — clears window → `WorldRenderer::DrawGrid()` → `WorldRenderer::DrawRobots()` → display

---

## 5. Every Class In Detail

### 5.1 `CellType` (enum in `Grid.h`)

```cpp
enum class CellType : uint8_t {
    Empty = 0,          // Walkable empty floor
    Wall,               // Blocks robot movement
    Shelf,              // Storage shelf (walkable for now)
    ChargingStation     // Charging station (walkable for now)
};
```

Currently only `Empty` and `Wall` affect gameplay. `Shelf` is rendered (brown) but has no special
logic. `ChargingStation` falls through to the `default` case in rendering (magenta).

---

### 5.2 `Grid` (Simulation/Grid.h + Grid.cpp)

A 2D tile grid stored as a **flat 1D vector** (row-major order).

**Header (complete interface):**
```cpp
class Grid {
public:
    Grid() = default;
    Grid(int width, int height);

    [[nodiscard]] CellType GetCell(int x, int y) const;
    void SetCell(int x, int y, CellType type);

    int GetWidth() const  { return m_width; }
    int GetHeight() const { return m_height; }

private:
    int m_width = 0;
    int m_height = 0;
    std::vector<CellType> m_cells;

    [[nodiscard]] int GetIndex(int x, int y) const;
};
```

**Implementation details:**
- Constructor: `m_cells.resize(width * height, CellType::Empty)` — all cells start empty
- `GetIndex(x, y)`: returns `y * m_width + x`, or `-1` if out of bounds
- `GetCell(x, y)`: returns `CellType::Wall` for out-of-bounds coordinates (safe default — robots
  can't walk off the grid)
- `SetCell(x, y, type)`: silently does nothing if coordinates are out of bounds

**Example usage:**
```cpp
Grid grid(20, 15);                          // 20 columns × 15 rows
grid.SetCell(5, 5, CellType::Wall);         // Place a wall at column 5, row 5
CellType c = grid.GetCell(5, 5);            // Returns CellType::Wall
CellType oob = grid.GetCell(-1, 0);         // Returns CellType::Wall (out of bounds)
```

---

### 5.3 `Robot` (Simulation/Robot.h + Robot.cpp)

A simple mobile agent with an ID and grid position.

**Header (complete interface):**
```cpp
class Robot {
public:
    Robot(int id, int startX, int startY);

    void TryMove(int dx, int dy, const Grid& grid);

    int getX() const;
    int getY() const;
    int setX(int x);      // Note: returns the new x value (not void)
    int setY(int y);      // Note: returns the new y value (not void)
    int getId() const;

private:
    int m_id;
    int m_x;
    int m_y;
    // float m_batteryLevel;    // <-- commented out, planned feature
    // bool m_isCarryingItem;   // <-- commented out, planned feature
};
```

**Implementation details:**
- Constructor: stores id/x/y and prints `"1. Robot created at 2,2"` to stdout
- `TryMove(dx, dy, grid)`:
  - Calculates target position: `(m_x + dx, m_y + dy)`
  - Checks `grid.GetCell(targetX, targetY)` — if `Wall`, prints blocked message and stays put
  - Otherwise, updates position to the target
  - No robot-vs-robot collision checking
- `setX()` / `setY()` return the assigned value (unusual pattern — `return m_x = x;`)

**Example usage:**
```cpp
Grid grid(20, 15);
grid.SetCell(3, 2, CellType::Wall);

Robot r(1, 2, 2);                    // Robot ID 1 at position (2,2)
r.TryMove(1, 0, grid);              // Try to move right to (3,2) → BLOCKED (wall)
r.TryMove(0, 1, grid);              // Try to move down to (2,3) → succeeds, now at (2,3)
```

---

### 5.4 `WorldRenderer` (Renderer/WorldRenderer.h + WorldRenderer.cpp)

Handles all SFML drawing. Converts grid coordinates to screen pixels.

**Header (complete interface):**
```cpp
class WorldRenderer {
public:
    WorldRenderer();

    void DrawGrid(sf::RenderWindow& window, const Grid& grid);
    void DrawRobots(sf::RenderWindow& window, const std::vector<Robot>& robots);

private:
    void DrawCell(sf::RenderWindow& window, int x, int y, CellType type);  // declared but NOT implemented
    float m_cellSize = 30.0f;       // Each grid cell = 30×30 pixels on screen
};
```

**Implementation details:**

- `DrawGrid()`:
  - Creates a `sf::RectangleShape` of size `(29, 29)` (1-pixel gap between cells)
  - Loops through every cell in the grid
  - Maps cell types to colors:
    - `Empty` → dark grey `(50, 50, 50)`
    - `Wall` → white
    - `Shelf` → brown `(139, 69, 19)`
    - `default` (including `ChargingStation`) → magenta (error/unhandled indicator)
  - Screen position: `x * 30, y * 30`

- `DrawRobots()`:
  - Draws each robot as a **cyan circle**
  - Circle radius: `m_cellSize * 0.8 / 2 = 12` pixels
  - Centered in the cell with an offset of `(30 - 24) / 2 = 3` pixels
  - Screen position: `robot.getX() * 30 + 3, robot.getY() * 30 + 3`

- `DrawCell()`: declared in the header but **never implemented** — dead code

**Coordinate conversion formula:**
```
screenX = gridX * 30.0f
screenY = gridY * 30.0f
```

---

### 5.5 `Application` (Core/Application.h + Application.cpp)

The main orchestrator. Owns the window, grid, renderer, and robots.

**Header (complete interface):**
```cpp
class Application {
public:
    Application();
    void Run();

private:
    void ProcessEvents();
    void Update();
    void Render();

    sf::RenderWindow m_window;
    Grid m_grid;
    WorldRenderer m_renderer;
    std::vector<Robot> m_robots;
    sf::Clock m_clock;              // declared but not used yet
};
```

**Implementation details:**

- **Constructor:**
  - Creates an 800×600 SFML window titled `"ARIS Simulation"`
  - Framerate limited to 60 FPS
  - Creates a 20×15 grid (so the grid covers `20*30 = 600px` wide, `15*30 = 450px` tall)
  - Places one wall at position `(5, 5)`
  - Spawns two robots: Robot 1 at `(2, 2)` and Robot 2 at `(4, 4)`

- **Run():** Classic game loop — `while (window.isOpen()) { ProcessEvents(); Update(); Render(); }`

- **ProcessEvents():**
  - Handles window close
  - Arrow keys move `m_robots[0]` (the first robot only):
    - Up → `TryMove(0, -1)`
    - Down → `TryMove(0, 1)`
    - Left → `TryMove(-1, 0)`
    - Right → `TryMove(1, 0)`

- **Update():** Empty — placeholder for future autonomous movement logic

- **Render():** `clear() → DrawGrid() → DrawRobots() → display()`

---

### 5.6 `main.cpp`

```cpp
int main() {
    Application app;
    app.Run();
    return 0;
}
```

There's also an unused `modifyCurrentWorkingDirectory()` function that walks up the directory tree
until it finds a `resources/` folder. It is declared but never called.

---

## 6. What's Currently Working (as of this document)

| Feature | Status | Details |
|---------|--------|---------|
| SFML window | ✅ Working | 800×600, 60 FPS, title "ARIS Simulation" |
| Grid system | ✅ Working | 20×15 cells, 4 cell types, bounds checking |
| Grid rendering | ✅ Working | Color-coded cells, 30px each, 1px gaps |
| Robot spawning | ✅ Working | 2 robots created at startup |
| Robot rendering | ✅ Working | Cyan circles centered in cells |
| Keyboard movement | ✅ Working | Arrow keys move Robot 0 |
| Wall collision | ✅ Working | Robots can't walk into Wall cells |
| Console logging | ✅ Working | Robot creation + movement logged to stdout |

---

## 7. What's NOT Implemented Yet

These features are mentioned in the README or code comments but do **not** exist in the codebase:

| Feature | Notes |
|---------|-------|
| **A\* Pathfinding** | No pathfinding code exists. Robots only move via manual keyboard input. |
| **Autonomous robot movement** | `Update()` is empty. Robots don't move on their own. |
| **Robot-to-robot collision** | Robots can overlap. No inter-robot collision detection. |
| **Battery system** | `m_batteryLevel` is commented out in Robot.h |
| **Cargo/item carrying** | `m_isCarryingItem` is commented out in Robot.h |
| **SQLite database** | Not included, not linked, no code exists |
| **Dear ImGui UI** | Not included, not linked, no code exists |
| **Order/Task management** | No `OrderManager` or task queue exists |
| **Heatmap visualization** | Not implemented |
| **Network/Server-Client** | Not implemented |
| **Textures/sprites** | All rendering uses primitive shapes (rectangles, circles) |
| **Delta-time movement** | `m_clock` is declared but never read |
| **`DrawCell()` method** | Declared in WorldRenderer.h but never implemented |
| **ChargingStation rendering** | Falls through to `default` case (magenta) — no specific color |

---

## 8. Key Design Decisions & Quirks

1. **Grid stored as 1D vector:** `m_cells[y * width + x]` — row-major. This is more cache-friendly
   than a vector-of-vectors.

2. **Out-of-bounds = Wall:** `GetCell()` returns `CellType::Wall` for any coordinate outside the
   grid. This means robots are implicitly blocked at grid edges — a neat trick.

3. **Setters return values:** `Robot::setX(int x)` returns `int` (the newly set value) rather
   than `void`. This is unusual but functional.

4. **SFML is built from source:** The SFML library is a Git submodule at `vendors/sfml/` and is
   compiled alongside the project. No system-installed SFML is needed.

5. **C++17 is set per-target, not globally:** Because SFML's audio module uses `std::auto_ptr`
   (removed in C++17), the standard is only applied to the main target.

6. **Header glob mismatch:** `GLOB_RECURSE` searches for `*.hpp` but all headers are `.h`. The
   headers are manually listed in `add_executable()` instead.

---

## 9. Example Scenarios for Another AI

### Example A: "Add a new cell type called `DropOff`"
You would need to:
1. Add `DropOff` to the `CellType` enum in `include/Simulation/Grid.h`
2. Add a color case for it in `WorldRenderer::DrawGrid()` in `src/Renderer/WorldRenderer.cpp`
3. Optionally place some DropOff cells in `Application::Application()` in `src/Core/Application.cpp`

### Example B: "Make Robot 2 move autonomously toward a target"
You would need to:
1. Add a target position or pathfinding method to the `Robot` class
2. Implement movement logic inside `Application::Update()` (currently empty)
3. Use `Robot::TryMove()` to move one step per frame toward the target

### Example C: "Implement A\* pathfinding"
You would need to:
1. Create new files: `include/Simulation/Pathfinder.h` and `src/Simulation/Pathfinder.cpp`
2. The pathfinder takes a `const Grid&` and computes a path from `(startX, startY)` to `(goalX, goalY)`
3. Use `Grid::GetCell()` to check walkability (anything that's not `Wall` is walkable)
4. Return a `std::vector<std::pair<int,int>>` as the path
5. In `Application::Update()`, pop the next step from the path and call `Robot::TryMove()`

### Example D: "Add battery drain to robots"
You would need to:
1. Uncomment `float m_batteryLevel;` in `Robot.h` and initialize it (e.g., `100.0f`)
2. Add a getter: `float getBattery() const;`
3. Decrease battery on each `TryMove()` call
4. Optionally render a battery bar in `WorldRenderer::DrawRobots()`
5. Add logic to stop the robot when battery reaches 0

### Example E: "Integrate Dear ImGui for a dashboard"
You would need to:
1. Add ImGui + ImGui-SFML as dependencies (submodule or vendored)
2. Update `CMakeLists.txt` to compile and link ImGui
3. Initialize ImGui in `Application::Application()`
4. Process ImGui events in `Application::ProcessEvents()`
5. Create ImGui windows in `Application::Render()` (between `clear()` and `display()`)
6. Show robot positions, grid stats, or inventory data in the ImGui panels

### Example F: "Add SQLite database for inventory"
You would need to:
1. Add SQLite3 as a dependency (system library or vendored)
2. Update `CMakeLists.txt`: `find_package(SQLite3)` and `target_link_libraries(... SQLite::SQLite3)`
3. Create `include/Database/DatabaseManager.h` and `src/Database/DatabaseManager.cpp`
4. Initialize the DB in `Application` constructor
5. Create tables for products, shelf locations, and order logs
6. Insert/query data when robots pick up or deliver items

---

## 10. Code Style Conventions

- **Naming:** Classes use `PascalCase`, member variables use `m_` prefix (`m_width`, `m_grid`),
  methods use `PascalCase` for public (`GetCell`, `TryMove`) and `camelCase` for getters/setters
  (`getX`, `setY`)
- **Header guards:** `#pragma once`
- **Includes:** Mix of `"relative/path.h"` and `<library>` includes
- **Comments:** Sparse, mostly inline explanations; author/date comments at top of each file
- **Braces:** Opening brace on same line for functions/classes; Allman style used occasionally
- **Const correctness:** Used well — `const Grid&` parameters, `const` getters, `[[nodiscard]]`

---

## 11. Dependencies

| Dependency | Version | How Included | Status |
|------------|---------|-------------|--------|
| SFML | 2.6 | Git submodule at `vendors/sfml/` | ✅ Active |
| SQLite3 | — | Not included | ❌ Planned |
| Dear ImGui | — | Not included | ❌ Planned |
| GoogleTest | — | Not included | ❌ Mentioned in README |

---

## 12. CI/CD Pipeline

There is a GitHub Actions workflow at `.github/workflows/build-release.yml`:
- **Triggers:** Git tags matching `v*` or published releases
- **Matrix:** Ubuntu, Windows, macOS
- **Steps:** Install deps → CMake configure → Build (Release) → Zip artifacts → Upload to GitHub Release
- **Linux deps installed:** libudev-dev, libopenal-dev, libvorbis-dev, libflac-dev, libxrandr-dev,
  libfreetype6-dev, libgl1-mesa-dev, libxcursor-dev

---

## 13. Quick Reference — All Files With Line Counts

| File | Lines | Key Contents |
|------|-------|-------------|
| `src/main.cpp` | 23 | Entry point, unused `modifyCurrentWorkingDirectory()` |
| `include/Core/Application.h` | 29 | Application class definition |
| `src/Core/Application.cpp` | 58 | Game loop, input handling, hardcoded grid/robot setup |
| `include/Simulation/Grid.h` | 33 | CellType enum, Grid class definition |
| `src/Simulation/Grid.cpp` | 30 | Grid bounds-checking, 1D indexing |
| `include/Simulation/Robot.h` | 28 | Robot class definition, commented-out battery/cargo |
| `src/Simulation/Robot.cpp` | 49 | TryMove with wall collision, console logging |
| `include/Renderer/WorldRenderer.h` | 27 | WorldRenderer class, unimplemented DrawCell |
| `src/Renderer/WorldRenderer.cpp` | 43 | Color-coded grid drawing, cyan circle robots |
