//
// Created by Devrim Yildiz on 15.01.26.
//

#pragma once
#include "Simulation/Grid.h"

class Robot {
public:
    Robot(int id, int startX, int startY);

    void TryMove(int dx, int dy, const Grid& grid);

    int getX() const;
    int getY() const;
    int setX(int x);
    int setY(int y);
    int getId() const;

private:
    int m_id;
    int m_x;
    int m_y;

    // float m_batteryLevel;
    // bool m_isCarryingItem;
};
