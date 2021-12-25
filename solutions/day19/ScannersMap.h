#pragma once

#include <map>
#include <Scanner.h>
#include <ScannersRotationCache.h>
#include <ScannersInnerDistancesCache.h>

namespace scanners_map {

    constexpr unsigned MIN_NUMBER_OF_OVERLAPPING_BEACONS = 12;

    std::vector<unsigned> get_initial_aligned_scanners(const std::vector<Scanner>& scanners) {
        return { scanners.at(0).id };
    }

    std::vector<unsigned> get_initial_unaligned_scanners(const std::vector<Scanner>& scanners) {
        std::vector<unsigned> unaligned_scanners{};
        std::transform(std::begin(scanners) + 1, std::end(scanners),
                       std::back_inserter(unaligned_scanners),
                       [](const Scanner& scanner){ return scanner.id; });
        return unaligned_scanners;
    }

    std::vector<unsigned> remove_aligned_scanners(std::vector<unsigned> unaligned_scanners, const std::vector<unsigned>& to_remove) {
        const auto remove_scanner = [&unaligned_scanners](unsigned scanner) {
            auto remove_it = std::remove(std::begin(unaligned_scanners), std::end(unaligned_scanners), scanner);
            unaligned_scanners.erase(remove_it, std::end(unaligned_scanners));
        };

        std::for_each(std::begin(to_remove), std::end(to_remove), remove_scanner);
        return unaligned_scanners;
    }

    bool is_overlapping_possible(unsigned scanner_a_id, unsigned scanner_b_id, const ScannersInnerDistancesCache& inner_distance_cache) {
        const auto& distances_for_scanner_a = inner_distance_cache.get_for_scanner(scanner_a_id);
        const auto& distances_for_scanner_b = inner_distance_cache.get_for_scanner(scanner_b_id);
        std::vector<unsigned> distances_intersection{};
        std::set_intersection(std::begin(distances_for_scanner_a), std::end(distances_for_scanner_a),
                              std::begin(distances_for_scanner_b), std::end(distances_for_scanner_b),
                              std::back_inserter(distances_intersection));
        return distances_intersection.size() >= (MIN_NUMBER_OF_OVERLAPPING_BEACONS * (MIN_NUMBER_OF_OVERLAPPING_BEACONS - 1) / 2);
    }

    std::vector<Point3D> move_by_offset(const std::vector<Point3D>& beacons, const Vector3D& offset) {
        const auto move_beacon = [&offset](const Point3D& beacon) {
            return Point3D{beacon.x + offset.x, beacon.y + offset.y, beacon.z + offset.z};
        };
        std::vector<Point3D> moved_beacons{};
        std::transform(std::begin(beacons), std::end(beacons), std::back_inserter(moved_beacons), move_beacon);
        return moved_beacons;
    }

    std::map<Vector3D, unsigned> create_offsets_histogram(const std::vector<Point3D>& scanner_a, const std::vector<Point3D>& scanner_b) {
        std::map<Vector3D, unsigned> offsets_histogram{};
        for(const auto& scanner_a_beacon: scanner_a) {
            for(const auto& scanner_b_beacon: scanner_b) {
                Vector3D offset{scanner_a_beacon.x - scanner_b_beacon.x, scanner_a_beacon.y - scanner_b_beacon.y, scanner_a_beacon.z - scanner_b_beacon.z};
                ++offsets_histogram[offset];
            }
        }
        return offsets_histogram;
    }

    std::optional<Vector3D> try_to_find_alignment_offset(const std::vector<Point3D>& scanner_a, const std::vector<Point3D>& scanner_b) {
        const auto offset_count_high_enough = [](const auto& kv_pair){
            return kv_pair.second >= MIN_NUMBER_OF_OVERLAPPING_BEACONS;
        };
        const auto offsets_histogram = create_offsets_histogram(scanner_a, scanner_b);
        const auto offset_it = std::find_if(std::begin(offsets_histogram), std::end(offsets_histogram), offset_count_high_enough);
        if(offset_it != std::end(offsets_histogram)) {
            return offset_it->first;
        }
        return std::nullopt;
    }

    std::optional<Scanner> try_to_align(const Scanner& scanner_a, const Scanner& scanner_b, const ScannersRotationCache& rotation_cache) {
        const auto& scanner_b_rotations = rotation_cache.get_for_scanner(scanner_b.id);
        for(const auto& rotated_scanner_b: scanner_b_rotations) {
            auto maybe_alignment_offset = try_to_find_alignment_offset(scanner_a.beacons, rotated_scanner_b);
            if(maybe_alignment_offset.has_value()) {
                auto alignment_offset = maybe_alignment_offset.value();
                auto transformed_beacons = move_by_offset(rotated_scanner_b, alignment_offset);
                return Scanner{scanner_b.id, transformed_beacons, alignment_offset};
            }
        }
        return std::nullopt;
    }

    std::vector<Scanner> align_scanners(std::vector<Scanner> scanners) {
        const auto scanners_rotations_cache = ScannersRotationCache::for_list_of_scanners(scanners);
        const auto scanners_inner_distances_cache = ScannersInnerDistancesCache::for_list_of_scanners(scanners);
        auto currently_processed_scanners = get_initial_aligned_scanners(scanners);
        auto unaligned_scanners = get_initial_unaligned_scanners(scanners);
        while(!unaligned_scanners.empty()) {
            std::vector<unsigned> newly_aligned_scanners{};
            for(unsigned aligned_scanner: currently_processed_scanners) {
                for(unsigned unaligned_scanner: unaligned_scanners) {
                    if(!is_overlapping_possible(aligned_scanner, unaligned_scanner, scanners_inner_distances_cache)) {
                        continue;
                    }
                    auto maybe_aligned_scanner = try_to_align(scanners.at(aligned_scanner), scanners.at(unaligned_scanner), scanners_rotations_cache);
                    if(maybe_aligned_scanner.has_value()) {
                        scanners.at(unaligned_scanner) = maybe_aligned_scanner.value();
                        newly_aligned_scanners.emplace_back(unaligned_scanner);
                    }
                }
                unaligned_scanners = remove_aligned_scanners(unaligned_scanners, newly_aligned_scanners);
            }
            currently_processed_scanners = newly_aligned_scanners;
        }
        return scanners;
    }

}