# A.R.I.S. – Automated Robot Inventory System

![C++](https://img.shields.io/badge/C++-17-blue.svg) ![Build](https://img.shields.io/badge/build-passing-brightgreen.svg) ![License](https://img.shields.io/badge/license-MIT-green.svg)

> **A.R.I.S.** (Automated Robot Inventory System) ist eine hochperformante Lager-Simulation, die autonome Roboter-Agenten, Pfadfindungs-Algorithmen und persistente Datenbank-Verwaltung in einer C++ Echtzeit-Anwendung vereint.

---

## 📖 Über das Projekt

Dieses Projekt demonstriert die Entwicklung komplexer **Simulations-Software** unter Verwendung moderner C++ Standards. Ziel war es, ein "Smart Warehouse" zu simulieren, in dem mehrere Roboter-Agenten effizient Aufträge abarbeiten, Kollisionen vermeiden und Inventarveränderungen in Echtzeit in einer SQL-Datenbank protokollieren.

Das System trennt strikt zwischen **Logik (Simulation)**, **Datenhaltung (SQLite)** und **Visualisierung (SFML/ImGui)**.

### ✨ Key Features

* **Multi-Agent System:** Steuerung mehrerer Roboter, die parallel Aufgaben erledigen.
* **Intelligentes Pathfinding:** Implementierung des **A* (A-Star)** Algorithmus zur dynamischen Wegfindung um Hindernisse.
* **Datenbank-Integration:** SQLite-Backend zur Verwaltung von Produkten, Lagerplätzen und Bestell-Logs.
* **Echtzeit-Dashboard:** Integration von **Dear ImGui** zur Anzeige von Live-Statistiken, Logs und Inventar-Tabellen.
* **Visuelle Simulation:** Flüssiges Rendering des Lager-Grids und der Agenten mittels **SFML**.

---

## 🛠 Tech Stack

* **Sprache:** C++17
* **Build System:** CMake (kompatibel mit `Make`, `Ninja`, CLion, VS Code)
* **Grafik & Windowing:** [SFML 2.6](https://www.sfml-dev.org/)
* **UI Overlay:** [Dear ImGui](https://github.com/ocornut/imgui)
* **Datenbank:** [SQLite3](https://www.sqlite.org/)
* **Testing:** GoogleTest (Optional)

---

## 🚀 Installation & Build

### Voraussetzungen

Stelle sicher, dass folgende Tools installiert sind:
* C++ Compiler (GCC, Clang oder MSVC)
* CMake (Version 3.15+)
* SFML Development Libraries
* SQLite3 Development Libraries

### 1. Repository klonen

```bash
git clone https://github.com/voricc/Cpp-ARIS.git
cd Cpp-ARIS
# Initialize submodules (SFML, ImGui, ImGui-SFML)
git submodule update --init --recursive
```

**Note:** This project uses git submodules for dependencies:
- **SFML 2.6**: Graphics and window management
- **ImGui 1.90.9**: UI library for the dashboard
- **ImGui-SFML 2.6.x**: ImGui bindings for SFML

### 2. Dependencies installieren

**Ubuntu / Debian:**

```bash
sudo apt-get install libx11-dev libxrandr-dev libxcursor-dev libxi-dev \
                     libudev-dev libgl1-mesa-dev libfreetype6-dev \
                     libopenal-dev libflac-dev libvorbis-dev libsqlite3-dev
```

**macOS (Homebrew):**

```bash
brew install sqlite
# SFML and ImGui are included as submodules
```

**Windows:**
Dependencies (SFML, ImGui, ImGui-SFML) are managed via git submodules. You'll need:
- Visual Studio 2019+ or MinGW-w64
- CMake 3.15+
- Git (to clone with submodules)

Additional system dependencies will be handled by SFML's CMake configuration.

### 3. Build (Terminal / Make)

```bash
mkdir build
cd build
cmake ..
make -j4
./bin/A.R.I.S
```

### 4. Nutzung in CLion (JetBrains)

1. Öffne den Ordner `Cpp-ARIS` in CLion.
2. CLion erkennt automatisch die `CMakeLists.txt` und lädt das Projekt.
3. Stelle sicher, dass die Submodules initialisiert sind:
   - `git submodule update --init --recursive`
4. Klicke auf **Build** (Hammer-Icon) und dann **Run** (Play-Button).

### Troubleshooting

**ImGui not found:**
- Ensure submodules are initialized: `git submodule update --init --recursive`
- The project uses ImGui v1.90.9 (compatible with imgui-sfml 2.6.x)

**SFML not found:**
- System SFML is not needed - it's included as a submodule
- Make sure all system dependencies listed above are installed

---

## 🏗 Architektur

Das Projekt folgt einer modularen Architektur (Separation of Concerns):

```text
ARIS/
├── src/
│   ├── Core/           # Hauptschleife (Game Loop) und Zeitmanagement
│   ├── Simulation/     # Logik: Grid, RobotAgent, Pathfinding (A*)
│   ├── Database/       # SQL-Wrapper und Query-Manager
│   ├── Renderer/       # SFML-Zeichenroutinen
│   └── UI/             # ImGui Dashboard-Code
├── include/            # Header-Dateien (.h)
├── assets/             # Texturen, Fonts, Datenbank-File (.db)
├── tests/              # Unit Tests
└── CMakeLists.txt      # Build Konfiguration

```

### Core-Konzepte

1. **Grid System:** Das Lager ist in Kacheln (Tiles) unterteilt. Jede Kachel hat Eigenschaften (Wand, Regal, Ladestation).
2. **Task Management:** Ein zentraler `OrderManager` holt offene Aufträge aus der Datenbank und weist sie dem nächsten verfügbaren Roboter zu (Producer-Consumer Prinzip).
3. **Persistenz:** Jede Warenbewegung wird transaktional in der SQLite-DB gespeichert (`COMMIT` erst nach erfolgreicher Lieferung).

---

## 🔜 Roadmap & TODOs

* [x] Grundlegendes Grid & Rendering
* [x] A* Pathfinding Implementierung
* [x] Roboter-Bewegung & Animation
* [x] ImGui Dashboard Integration
* [ ] **Heatmap-Visualisierung:** Anzeige stark frequentierter Wege
* [ ] **Collision Avoidance:** Erweiterte Logik, damit Roboter einander ausweichen
* [ ] **Netzwerk-Support:** Trennung von Server (Simulation) und Client (Viewer)
* [ ] **Performance Metrics:** Detaillierte Analyse der Roboter-Effizienz
* [ ] **Dynamic Task Prioritization:** Priorisierung von dringenden Aufträgen
* [ ] **Battery Optimization:** Intelligente Ladestrategien für Roboter
* [ ] **Multi-Floor Support:** Unterstützung für mehrstöckige Lager mit Aufzügen
* [ ] **Path Prediction:** Vorhersage von Roboter-Routen zur Vermeidung von Staus
* [ ] **Configuration System:** Externe Konfigurationsdateien (JSON/YAML) für Grid und Roboter-Parameter
* [ ] **Replay System:** Aufzeichnung und Wiedergabe von Simulationen
* [ ] **REST API:** HTTP-Schnittstelle für externe Steuerung und Monitoring

---

## 🤝 Contributing

Pull Requests sind willkommen! Für größere Änderungen öffne bitte zuerst ein Issue, um die Änderung zu diskutieren.

## 📄 Lizenz

Dieses Projekt ist unter der MIT Lizenz veröffentlicht - siehe [LICENSE](https://www.google.com/search?q=LICENSE) Datei für Details.

---

**Entwickelt von voricc (Devrim Yildiz)**

```

```
