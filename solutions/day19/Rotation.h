#pragma once

#include <Scanner.h>

namespace rotation {

    enum class Axis {
        x_axis, y_axis, z_axis
    };

    Point3D rotate_along_origin_axis(const Point3D& beacon, const Axis& axis) {
        Point3D new_beacon_position{};
        if(axis == Axis::x_axis) {
            new_beacon_position.x = beacon.x; // x
            new_beacon_position.y = -beacon.z; // y * cos(90) - z * sin(90)
            new_beacon_position.z = beacon.y; // y * sin(90) + z * cos(90)
        }
        else if(axis == Axis::y_axis) {
            new_beacon_position.x = beacon.z; // x * cos(90) + z * sin(90)
            new_beacon_position.y = beacon.y; // y
            new_beacon_position.z = -beacon.x; // z * cos(90) - x * sin(90)
        }
        else {
            new_beacon_position.x = -beacon.y; // x * cos(90) - y * sin(90)
            new_beacon_position.y = beacon.x; // x * sin(90) + y * cos(90)
            new_beacon_position.z = beacon.z; // z
        }
        return new_beacon_position;
    }

    std::vector<Point3D> rotate_along_origin_axis(const std::vector<Point3D>& beacons, const Axis& axis) {
        std::vector<Point3D> new_beacon_positions{};
        for(const auto& beacon: beacons) {
            const auto new_beacon_position = rotate_along_origin_axis(beacon, axis);
            new_beacon_positions.emplace_back(new_beacon_position);
        }
        return new_beacon_positions;
    }

    std::vector<std::vector<Point3D>> generate_all_possible_rotations(std::vector<Point3D> beacons) {
        static const std::vector<Axis> full_rotation_sequence{ Axis::x_axis, Axis::x_axis, Axis::z_axis, Axis::x_axis, Axis::x_axis, Axis::z_axis };
        std::vector<std::vector<Point3D>> rotations{};
        for(const auto& rotation: full_rotation_sequence) {
            for(unsigned i = 0; i < 4; ++i) {
                rotations.push_back(beacons);
                beacons = rotate_along_origin_axis(beacons, Axis::y_axis);
            }
            beacons = rotate_along_origin_axis(beacons, rotation);
        }
        return rotations;
    }

}