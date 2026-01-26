//
// Created by Devrim Yildiz on 15.01.26.
//

#pragma once
#include <vector>
#include <cstdint>
#include "Simulation/TileTypes.h"

class Grid {
public:
    Grid() = default; // needed for Application member initialization
    Grid(int width, int height);

    [[nodiscard]] CellType GetCell(int x, int y) const;
    void SetCell(int x, int y, CellType type);

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    
    bool IsWalkable(int x, int y) const;
    void IncrementTraffic(int x, int y);
    int GetTraffic(int x, int y) const;
    void ResetTraffic();

private:
    int m_width = 0;
    int m_height = 0;
    std::vector<CellType> m_cells;
    std::vector<int> m_traffic;

    [[nodiscard]] int GetIndex(int x, int y) const;
};