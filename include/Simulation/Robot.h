//
// Created by Devrim Yildiz on 15.01.26.
//

#ifndef A_R_I_S_ROBOT_H
#define A_R_I_S_ROBOT_H
#pragma once

class Robot {
public:
    Robot(int id, int startX, int startY);

    void move(int dx, int dy);

    void setPosition(int x, int y);

    int getX() const;
    int getY() const;
    int getId() const;

private:
    int m_id;
    int m_x;
    int m_y;


    // float m_batteryLevel;
    // bool m_isCarryingItem;
};
#endif //A_R_I_S_ROBOT_H