#pragma once

#include <map>
#include <Scanner.h>
#include <ScannersRotationCache.h>
#include <ScannersInnerDistancesCache.h>

namespace beacons_map {

    std::set<Point3D> get_unique_beacon_positions(const std::vector<Scanner>& scanners) {
        std::set<Point3D> unique_beacons{};
        for(const auto& scanner: scanners) {
            unique_beacons.insert(std::begin(scanner.beacons), std::end(scanner.beacons));
        }
        return unique_beacons;
    }

}