#include <iostream>
#include <vector>
#include <set>
#include <ScannersMap.h>
#include <BeaconsMap.h>
#include <InputParser.h>


unsigned solve_part_one(const std::vector<Scanner>& scanners) {
    const auto aligned_scanners = scanners_map::align_scanners(scanners);
    const auto beacons_positions = beacons_map::get_unique_beacon_positions(aligned_scanners);
    return beacons_positions.size();
}

unsigned solve_part_two(const std::vector<Scanner>& scanners) {
    const auto aligned_scanners = scanners_map::align_scanners(scanners);
    return manhattan_distance::max_between_scanners(aligned_scanners);
}

int main() {
    const auto puzzle_input = input_parser::read_puzzle_input("input.txt");
    std::cout << "Part 1: " << solve_part_one(puzzle_input) << std::endl;
    std::cout << "Part 2: " << solve_part_two(puzzle_input) << std::endl;
    return 0;
}