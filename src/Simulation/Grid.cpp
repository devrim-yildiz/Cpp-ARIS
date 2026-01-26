//
// Created by Devrim Yildiz on 26.01.26.
//
#include "Simulation/Grid.h"

Grid::Grid(int width, int height)
    : m_width(width), m_height(height) {
    m_cells.resize(width * height, CellType::Empty);
    m_traffic.resize(width * height, 0);
}

int Grid::GetIndex(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) return -1;
    return y * m_width + x;
}

CellType Grid::GetCell(int x, int y) const {
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

bool Grid::IsWalkable(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) return false;
    CellType type = GetCell(x, y);
    return type == CellType::Empty || type == CellType::ChargingStation || 
           type == CellType::PickupZone || type == CellType::DropZone;
}

void Grid::IncrementTraffic(int x, int y) {
    int idx = GetIndex(x, y);
    if (idx != -1) m_traffic[idx]++;
}

int Grid::GetTraffic(int x, int y) const {
    int idx = GetIndex(x, y);
    if (idx == -1) return 0;
    return m_traffic[idx];
}

void Grid::ResetTraffic() {
    std::fill(m_traffic.begin(), m_traffic.end(), 0);
}