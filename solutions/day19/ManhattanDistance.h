#pragma once

#include <Scanner.h>

namespace manhattan_distance {

    unsigned between_two_points(const Point3D& point_a, const Point3D& point_b) {
        return std::abs(point_a.x - point_b.x) + std::abs(point_a.y - point_b.y) + std::abs(point_a.z - point_b.z);
    }

    unsigned max_between_scanners(const std::vector<Scanner>& scanners) {
        unsigned max_distance = 0;
        for(unsigned i = 0; i < scanners.size() - 1; ++i) {
            for(unsigned j = i + 1; j < scanners.size(); ++j) {
                unsigned distance = between_two_points(scanners.at(i).position, scanners.at(j).position);
                max_distance = std::max(max_distance, distance);
            }
        }
        return max_distance;
    }
}