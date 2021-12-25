#pragma once

struct Point3D {
    int x = 0;
    int y = 0;
    int z = 0;
};

using Vector3D = Point3D;

bool operator<(const Point3D& lhs, const Point3D& rhs) {
    if(lhs.x < rhs.x) return true;
    else if(lhs.x > rhs.x) return false;
    if(lhs.y < rhs.y) return true;
    else if(lhs.y > rhs.y) return false;
    return lhs.z < rhs.z;
}

struct Scanner {
    unsigned id = 0;
    std::vector<Point3D> beacons{};
    Point3D position{0, 0, 0};
};