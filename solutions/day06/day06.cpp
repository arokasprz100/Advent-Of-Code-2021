#include <fstream>
#include <iostream>
#include <vector>
#include <numeric>

#include <Utils.h>

constexpr unsigned NEW_FISH_STARTING_TIMER_VALUE{8};
constexpr unsigned FISH_TIMER_RESET_VALUE{6};

std::vector<unsigned> read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    std::string input_line{};
    std::getline(file, input_line);
    return utils::comma_separated_string_to_numbers(input_line);
}

long long int calculate_lantern_fish_colony_size(const std::vector<unsigned>& initial_state, unsigned number_of_days) {
    std::vector<long long int> fish_timer_value_counts(NEW_FISH_STARTING_TIMER_VALUE + 1, 0ll);
    for(auto& fish_timer : initial_state) {
        fish_timer_value_counts[fish_timer]++;
    }
    for(int day = 0; day < number_of_days; ++day) {
        long long int number_of_fish_to_create = fish_timer_value_counts[0];
        for(unsigned timer_value = 1; timer_value <= NEW_FISH_STARTING_TIMER_VALUE; ++timer_value) {
            fish_timer_value_counts[timer_value - 1] = fish_timer_value_counts[timer_value];
        }
        fish_timer_value_counts[NEW_FISH_STARTING_TIMER_VALUE] = number_of_fish_to_create;
        fish_timer_value_counts[FISH_TIMER_RESET_VALUE] += number_of_fish_to_create;
    }
    return std::accumulate(std::begin(fish_timer_value_counts), std::end(fish_timer_value_counts),0ll);
}

int main() {
    const auto puzzle_input = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << calculate_lantern_fish_colony_size(puzzle_input, 80) << std::endl;
    std::cout << "Part 2: " << calculate_lantern_fish_colony_size(puzzle_input, 256) << std::endl;
    return 0;
}