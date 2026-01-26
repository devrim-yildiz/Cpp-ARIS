#pragma once
#include <cstdint>

enum class CellType : uint8_t {
    Empty = 0,
    Wall,
    Shelf,
    ChargingStation,
    PickupZone,
    DropZone
};

enum class RobotState : uint8_t {
    IDLE = 0,
    MOVING_TO_PICKUP,
    PICKING_UP,
    MOVING_TO_DROP,
    DROPPING,
    LOW_BATTERY,
    CHARGING
};
