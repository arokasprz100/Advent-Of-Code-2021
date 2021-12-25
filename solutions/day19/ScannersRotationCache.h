#pragma once

#include <Scanner.h>
#include <Rotation.h>

class ScannersRotationCache {
public:
    using PossibleRotation = std::vector<Point3D>;

    static ScannersRotationCache for_list_of_scanners(const std::vector<Scanner>& scanners) {
        ScannersRotationCache rotation_cache{};
        for(const auto& scanner: scanners) {
            rotation_cache.append_for_scanner(scanner);
        }
        return rotation_cache;
    }

    void append_for_scanner(const Scanner& scanner) {
        auto all_possible_rotations = rotation::generate_all_possible_rotations(scanner.beacons);
        rotations_by_scanner_ids.emplace(scanner.id, std::move(all_possible_rotations));
    }

    [[nodiscard]]
    const std::vector<PossibleRotation>& get_for_scanner(unsigned scanner_id) const {
        return rotations_by_scanner_ids.at(scanner_id);
    }

private:
    std::map<unsigned, std::vector<PossibleRotation>> rotations_by_scanner_ids{};
};
