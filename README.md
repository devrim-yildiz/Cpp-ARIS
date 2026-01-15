# A.R.I.S. – Automated Robot Inventory System

![C++](https://img.shields.io/badge/C++-17-blue.svg) ![Build](https://img.shields.io/badge/build-passing-brightgreen.svg) ![License](https://img.shields.io/badge/license-MIT-green.svg)

> **A.R.I.S.** (Automated Robot Inventory System) ist eine hochperformante Lager-Simulation, die autonome Roboter-Agenten, Pfadfindungs-Algorithmen und persistente Datenbank-Verwaltung in einer C++ Echtzeit-Anwendung vereint.

![ARIS Screenshot](docs/screenshot_placeholder.png)
*(Hinweis: Screenshot der Simulation hier einfügen)*

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
git clone [https://github.com/DEIN_USERNAME/ARIS.git](https://github.com/DEIN_USERNAME/ARIS.git)
cd ARIS

```

### 2. Dependencies installieren

**Ubuntu / Debian:**

```bash
sudo apt-get install libsfml-dev libsqlite3-dev

```

**macOS (Homebrew):**

```bash
brew install sfml sqlite

```

**Windows:**
Am einfachsten ist die Nutzung eines Paketmanagers wie **vcpkg** oder die Konfiguration über CMake in CLion.

### 3. Build (Terminal / Make)

```bash
mkdir build
cd build
cmake ..
make
./ARIS

```

### 4. Nutzung in CLion (JetBrains)

1. Öffne den Ordner `ARIS` in CLion.
2. CLion erkennt automatisch die `CMakeLists.txt` und lädt das Projekt.
3. Falls SFML nicht automatisch gefunden wird, füge unter `File -> Settings -> Build, Execution, Deployment -> CMake` in den **CMake Options** folgendes hinzu:
`-DSFML_DIR=Pfad/zu/SFML/lib/cmake/SFML`
4. Klicke auf **Build** (Hammer-Icon) und dann **Run** (Play-Button).

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

* [] Grundlegendes Grid & Rendering
* [] A* Pathfinding Implementierung
* [] Roboter-Bewegung & Animation
* [ ] **Heatmap-Visualisierung:** Anzeige stark frequentierter Wege
* [ ] **Collision Avoidance:** Erweiterte Logik, damit Roboter einander ausweichen
* [ ] **Netzwerk-Support:** Trennung von Server (Simulation) und Client (Viewer)

---

## 🤝 Contributing

Pull Requests sind willkommen! Für größere Änderungen öffne bitte zuerst ein Issue, um die Änderung zu diskutieren.

## 📄 Lizenz

Dieses Projekt ist unter der MIT Lizenz veröffentlicht - siehe [LICENSE](https://www.google.com/search?q=LICENSE) Datei für Details.

---

**Entwickelt von voricc (Devrim Yildiz)**

```

```
