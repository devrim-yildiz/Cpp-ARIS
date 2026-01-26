#pragma once
#include <cmath>
#include <vector>

namespace Utils {

struct Point {
    int x, y;
    
    Point() : x(0), y(0) {}
    Point(int x_, int y_) : x(x_), y(y_) {}
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Point& other) const {
        return !(*this == other);
    }
};

inline int ManhattanDistance(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

inline int ManhattanDistance(const Point& a, const Point& b) {
    return ManhattanDistance(a.x, a.y, b.x, b.y);
}

inline float EuclideanDistance(int x1, int y1, int x2, int y2) {
    int dx = x1 - x2;
    int dy = y1 - y2;
    return std::sqrt(static_cast<float>(dx * dx + dy * dy));
}

inline float EuclideanDistance(const Point& a, const Point& b) {
    return EuclideanDistance(a.x, a.y, b.x, b.y);
}

}
