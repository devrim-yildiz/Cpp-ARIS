//
// Created by Devrim Yildiz on 15.01.26.
//
#include "Simulation/Robot.h"
#include "Simulation/Grid.h"
#include <iostream>
#include <ostream>

Robot::Robot(int id, int startX, int startY)
    : m_id(id), m_x(startX), m_y(startY)
{
    std::cout << m_id << ". Robot created at " << m_x << "," << m_y << std::endl;
}

void Robot::TryMove(int dx, int dy, const Grid& grid) {
    int currentX = this->getX();
    int targetX = currentX + dx;
    int currentY = this->getY();
    int targetY = currentY + dy;

    if (grid.GetCell(targetX,targetY) == CellType::Wall) {
        std::cout << "Robot " << m_id << " blocked by Wall at " << targetX << "," << targetY << std::endl;
    } else
    {
        this->setX(targetX);
        this->setY(targetY);
    }

}

int Robot::getX() const {
    return m_x;
}

int Robot::getY() const {
    return m_y;
}

int Robot::setX(int x) {
    return m_x = x;
}

int Robot::setY(int y) {
    return m_y = y;
}

int Robot::getId() const {
    return m_id;
}