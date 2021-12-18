#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <numeric>

#include <Utils.h>

using Heightmap = std::vector<std::vector<unsigned>>;
using Coordinates = std::pair<unsigned, unsigned>;

Heightmap read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    Heightmap puzzle_input{};
    for(std::string input_line; std::getline(file, input_line); ) {
        input_line = utils::trim(input_line);
        std::vector<unsigned> input_row{};
        std::transform(std::begin(input_line), std::end(input_line), std::back_inserter(input_row),
                       [](char to_convert) { return static_cast<unsigned>(to_convert - '0'); });
        puzzle_input.emplace_back(input_row);
    }
    return puzzle_input;
}

Heightmap add_boundaries(const Heightmap& heightmap, unsigned boundary_value) {
    Heightmap heightmap_with_boundaries{};
    unsigned number_of_entries_in_a_row = heightmap.at(0).size();
    std::vector<unsigned> first_or_last_row(number_of_entries_in_a_row + 2, 9);
    heightmap_with_boundaries.push_back(first_or_last_row);
    for(const auto& row: heightmap) {
        std::vector<unsigned> new_row(number_of_entries_in_a_row + 2, boundary_value);
        std::copy(std::begin(row), std::end(row), std::begin(new_row) + 1);
        heightmap_with_boundaries.push_back(new_row);
    }
    heightmap_with_boundaries.push_back(first_or_last_row);
    return heightmap_with_boundaries;
}

bool is_low_point(unsigned x, unsigned y, const Heightmap& heightmap_with_boundaries) {
    unsigned value = heightmap_with_boundaries.at(y).at(x);
    return value < heightmap_with_boundaries.at(y - 1).at(x) &&
           value < heightmap_with_boundaries.at(y + 1).at(x) &&
           value < heightmap_with_boundaries.at(y).at(x - 1) &&
           value < heightmap_with_boundaries.at(y).at(x + 1);
}

std::vector<Coordinates> find_low_points_coordinates(const Heightmap& heightmap) {
    const auto heightmap_with_boundaries = add_boundaries(heightmap, 9);
    std::vector<Coordinates> low_points{};
    for(unsigned y = 1; y < heightmap_with_boundaries.size() - 1; ++y) {
        for(unsigned x = 1; x < heightmap_with_boundaries.at(0).size() - 1; ++x) {
            if(is_low_point(x, y, heightmap_with_boundaries)) {
                low_points.emplace_back(x - 1, y - 1);
            }
        }
    }
    return low_points;
}

unsigned calculate_risk_for_low_points(const std::vector<Coordinates>& low_points, const Heightmap& heightmap) {
    const auto risk_accumulator = [&heightmap](unsigned previous_sum, const Coordinates& point) {
        return previous_sum + heightmap.at(point.second).at(point.first) + 1;
    };
    return std::accumulate(std::begin(low_points), std::end(low_points), 0, risk_accumulator);
}

void classify_heightmap_point_and_neighbours(const Coordinates& point_to_check, std::set<Coordinates>& processed_points, const Heightmap& heightmap) {
    if(processed_points.find(point_to_check) != std::end(processed_points)) return;
    if(heightmap.at(point_to_check.second).at(point_to_check.first) == 9) return;
    processed_points.insert(point_to_check);
    classify_heightmap_point_and_neighbours({point_to_check.first + 1, point_to_check.second}, processed_points, heightmap);
    classify_heightmap_point_and_neighbours({point_to_check.first - 1, point_to_check.second}, processed_points, heightmap);
    classify_heightmap_point_and_neighbours({point_to_check.first, point_to_check.second + 1}, processed_points, heightmap);
    classify_heightmap_point_and_neighbours({point_to_check.first, point_to_check.second - 1}, processed_points, heightmap);
}

unsigned calculate_basin_size_for_given_low_point(const Coordinates& low_point, const Heightmap& heightmap) {
    std::set<Coordinates> processed_points{};
    classify_heightmap_point_and_neighbours(low_point, processed_points, heightmap);
    return processed_points.size();
}

std::vector<unsigned> calculate_basins_sizes(const std::vector<Coordinates>& low_points, const Heightmap& heightmap) {
    const auto heightmap_with_boundaries = add_boundaries(heightmap, 9);
    std::vector<unsigned> basins_sizes{};
    for(const auto& point: low_points) {
        const Coordinates point_with_boundaries{point.first + 1, point.second + 1};
        unsigned basin_size = calculate_basin_size_for_given_low_point(point_with_boundaries, heightmap_with_boundaries);
        basins_sizes.emplace_back(basin_size);
    }
    return basins_sizes;
}

unsigned multiply_sizes_of_three_largest_basins(const std::vector<unsigned>& basins_sizes) {
    std::vector<unsigned> sorted_sizes(basins_sizes);
    std::sort(std::begin(sorted_sizes), std::end(sorted_sizes), std::greater<>());
    return sorted_sizes.at(0) * sorted_sizes.at(1) * sorted_sizes.at(2);
}

unsigned solve_part_one(const Heightmap& heightmap) {
    const auto low_points = find_low_points_coordinates(heightmap);
    return calculate_risk_for_low_points(low_points, heightmap);
}

unsigned solve_part_two(const Heightmap& heightmap) {
    const auto low_points = find_low_points_coordinates(heightmap);
    const auto basins_sizes = calculate_basins_sizes(low_points, heightmap);
    return multiply_sizes_of_three_largest_basins(basins_sizes);
}

int main() {
    const auto puzzle_input = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << solve_part_one(puzzle_input) << std::endl;
    std::cout << "Part 2: " << solve_part_two(puzzle_input) << std::endl;
    return 0;
}