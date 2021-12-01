#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>

std::vector<int> read_input_file(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    int current_value{};
    std::vector<int> input_from_file{};
    while(file >> current_value) {
        input_from_file.push_back(current_value);
    }
    return input_from_file;
}

int solve_part_one(const std::vector<int>& puzzle_input) {
    int depth_measurement_increases{0};
    for(int i = 1; i < puzzle_input.size(); ++i) {
        depth_measurement_increases = puzzle_input.at(i) > puzzle_input.at(i - 1) ? depth_measurement_increases + 1 : depth_measurement_increases;
    }
    return depth_measurement_increases;
}

int solve_part_two(const std::vector<int>& puzzle_input) {
    std::vector<int> measurement_sums{};
    measurement_sums.reserve(puzzle_input.size() - 2);
    for(int i = 0; i < puzzle_input.size() - 2; ++i) {
        measurement_sums.push_back(puzzle_input[i] + puzzle_input[i + 1] + puzzle_input[i + 2]);
    }
    return solve_part_one(measurement_sums);
}

int main() {
    const auto puzzle_input = read_input_file("input.txt");
    std::cout << "Part 1: " << solve_part_one(puzzle_input) << std::endl;
    std::cout << "Part 2: " << solve_part_two(puzzle_input) << std::endl;
    return 0;
}

