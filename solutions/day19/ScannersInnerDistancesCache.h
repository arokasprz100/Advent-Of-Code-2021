#pragma once

#include <Scanner.h>
#include <ManhattanDistance.h>

class ScannersInnerDistancesCache {
public:
    using InternalDistances = std::vector<unsigned>;

    static ScannersInnerDistancesCache for_list_of_scanners(const std::vector<Scanner>& scanners) {
        ScannersInnerDistancesCache inner_distances_cache{};
        for(const auto& scanner: scanners) {
            inner_distances_cache.append_for_scanner(scanner);
        }
        return inner_distances_cache;
    }

    void append_for_scanner(const Scanner& scanner) {
        auto distances = compute_internal_distances(scanner);
        internal_distances_by_scanner_ids.emplace(scanner.id, std::move(distances));
    }

    [[nodiscard]]
    const InternalDistances& get_for_scanner(unsigned scanner_id) const {
        return internal_distances_by_scanner_ids.at(scanner_id);
    }

private:
    std::map<unsigned, InternalDistances> internal_distances_by_scanner_ids{};

    static InternalDistances compute_internal_distances(const Scanner& scanner) {
        InternalDistances distances{};
        for(unsigned i = 0; i < scanner.beacons.size() - 1; ++i) {
            for(unsigned j = i + 1; j < scanner.beacons.size(); ++j) {
                distances.push_back(manhattan_distance::between_two_points(scanner.beacons.at(i), scanner.beacons.at(j)));
            }
        }
        std::sort(std::begin(distances), std::end(distances));
        return distances;
    }
};