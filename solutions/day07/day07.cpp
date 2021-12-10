#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>
#include <limits>
#include <functional>
#include <cmath>

#include <Utils.h>

using FuelCostCalculator = std::function<unsigned(const std::vector<unsigned>&, unsigned)>;

std::vector<unsigned> read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    std::string input_line{};
    std::getline(file, input_line);
    return utils::comma_separated_string_to_numbers(input_line);
}

unsigned finite_arithmetic_progression_sum(unsigned first, unsigned how_many, unsigned difference) {
    return std::lround((how_many / 2.0) * (2.0 * first + (how_many - 1.0) * difference));
}

unsigned calculate_constant_rate_fuel_cost_for_given_position(const std::vector<unsigned>& crab_positions, unsigned position) {
    auto distance_accumulator = [&position](unsigned distance_sum, unsigned crab_position) {
        return distance_sum + std::abs(static_cast<int>(crab_position) - static_cast<int>(position));
    };
    return std::accumulate(std::begin(crab_positions), std::end(crab_positions), 0, distance_accumulator);
}

unsigned calculate_changing_rate_fuel_cost_for_given_position(const std::vector<unsigned>& crab_positions, unsigned position) {
    auto distance_accumulator = [&position](unsigned distance_sum, unsigned crab_position) {
        unsigned current_distance = std::abs(static_cast<int>(crab_position) - static_cast<int>(position));
        return distance_sum + finite_arithmetic_progression_sum(1, current_distance, 1);
    };
    return std::accumulate(std::begin(crab_positions), std::end(crab_positions), 0, distance_accumulator);
}

unsigned find_minimal_fuel_cost(const std::vector<unsigned>& puzzle_input, const FuelCostCalculator& fuel_cost_calculator) {
    unsigned max_crab_position = *std::max_element(std::begin(puzzle_input), std::end(puzzle_input));
    unsigned min_crab_position = *std::min_element(std::begin(puzzle_input), std::end(puzzle_input));
    unsigned minimal_fuel_cost = std::numeric_limits<unsigned>::max();
    for(unsigned possible_position = min_crab_position; possible_position <= max_crab_position; ++possible_position) {
        unsigned fuel_cost = fuel_cost_calculator(puzzle_input, possible_position);
        minimal_fuel_cost = fuel_cost < minimal_fuel_cost ? fuel_cost : minimal_fuel_cost;
    }
    return minimal_fuel_cost;
}

int main() {
    const auto puzzle_input = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << find_minimal_fuel_cost(puzzle_input, calculate_constant_rate_fuel_cost_for_given_position) << std::endl;
    std::cout << "Part 2: " << find_minimal_fuel_cost(puzzle_input, calculate_changing_rate_fuel_cost_for_given_position) << std::endl;
    return 0;
}