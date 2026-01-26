//
// Created by Devrim Yildiz on 26.01.26.
//
#include "Simulation/Grid.h"

Grid::Grid(int width, int height)
    : m_width(width), m_height(height) {
    m_cells.resize(width * height, CellType::Empty);
}

int Grid::GetIndex(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) return -1;
    return y * m_width + x;
}

CellType Grid::GetCell(int x, int y) const {
    // bounds checkkk
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return CellType::Wall;
    }

    int index = GetIndex(x, y);

    return m_cells[index];
}

void Grid::SetCell(int x, int y, CellType type) {
    int idx = GetIndex(x, y);
    if (idx != -1) m_cells[idx] = type;
}