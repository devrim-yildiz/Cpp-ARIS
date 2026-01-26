//
// Created by Devrim Yildiz on 15.01.26.
//

#ifndef A_R_I_S_GRID_H
#define A_R_I_S_GRID_H
#pragma once
#include <vector>
#include <cstdint>

enum class CellType : uint8_t {
    Empty = 0,
    Wall,
    Shelf,
    ChargingStation
};

class Grid {
public:
    Grid() = default; // needed for Application member initialization
    Grid(int width, int height);

    [[nodiscard]] CellType GetCell(int x, int y) const;
    void SetCell(int x, int y, CellType type);

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    int m_width = 0;
    int m_height = 0;
    std::vector<CellType> m_cells;

    [[nodiscard]] int GetIndex(int x, int y) const;
};
#endif //A_R_I_S_GRID_H