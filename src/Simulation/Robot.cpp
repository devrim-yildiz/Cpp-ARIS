//
// Created by Devrim Yildiz on 15.01.26.
//
#include "Simulation/Robot.h"

#include <iostream>
#include <ostream>

Robot::Robot(int id, int startX, int startY)
    : m_id(id), m_x(startX), m_y(startY)
{
    int count = 0;
    std::cout<< count + ". Robot::Robot() created" <<std::endl;
}

// 2. Move Funktion
void Robot::move(int dx, int dy) {
    m_x += dx; // x = x + dx
    m_y += dy; // y = y + dy

    // if (m_x < 0) m_x = 0;
}

void Robot::setPosition(int x, int y) {
    m_x = x;
    m_y = y;
}

int Robot::getX() const {
    return m_x;
}

int Robot::getY() const {
    return m_y;
}

int Robot::getId() const {
    return m_id;
}