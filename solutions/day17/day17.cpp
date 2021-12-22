#include <iostream>
#include <fstream>
#include <regex>

using Range = std::pair<int, int>;

std::pair<Range, Range> read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    std::regex puzzle_input_regex{"target area: x=(-?[0-9]+)..(-?[0-9]+), y=(-?[0-9]+)..(-?[0-9]+)"};
    std::string input_line{};
    std::getline(file, input_line);
    if(std::smatch matches{}; std::regex_search(input_line, matches, puzzle_input_regex)) {
        Range x_range{std::stoi(matches[1].str()), std::stoi(matches[2].str())};
        Range y_range{std::stoi(matches[3].str()), std::stoi(matches[4].str())};
        return {x_range, y_range};
    }
    throw std::runtime_error{"Input does not match required pattern"};
}

bool target_area_below_zero(const Range& y_range) {
    return y_range.first < 0 && y_range.second < 0;
}

bool target_area_to_the_right(const Range& x_range) {
    return x_range.first > 0 && x_range.second > 0;
}

int max_possible_initial_y_velocity(const Range& y_range) {
    return std::abs(y_range.first) - 1;
}

// We assume that the target area is located below the initial position
// (duh, it is in an ocean trench...)  - any other type of input is not accepted.
// When shooting upwards, velocity will slowly decrease until reaching zero, then the
// probe will return to the same y position it was launched from (so, y = 0). This is
// because its velocity increases at the same rate it decreased when traveling upwards
// (for example: 4, 3, 2, 1, 0, -1, -2, -3, -4). When the probe reaches 0, it must not
// overshoot, so it can not travel faster than -(abs(y_lower_bound) - 1) at this point.
// Max height is just the sum: v0 + v1 + ... 0, (for example 4 + 3 + 2 + 1 + 0)
int max_possible_height(const Range& y_range) {
    if(!target_area_below_zero(y_range)) {
        throw std::runtime_error{"Invalid input - only negative y ranges supported."};
    }
    int max_possible_initial_velocity = max_possible_initial_y_velocity(y_range);
    return max_possible_initial_velocity * (max_possible_initial_velocity + 1) / 2;
}

bool target_area_reached(const Range& x_range, const Range& y_range, const std::pair<int, int>& position) {
    return position.first >= x_range.first && position.first <= x_range.second && position.second >= y_range.first && position.second <= y_range.second;
}

bool target_area_possible_to_reach(const Range& x_range, const Range& y_range, int vx, int vy) {
    std::pair<int, int> position{0, 0};
    while(position.first <= x_range.second && position.second >= y_range.first) {
        position.first += vx;
        position.second += vy;
        if(target_area_reached(x_range, y_range, position)) {
            return true;
        }
        if(vx != 0) {
            vx = (vx > 0 ? vx - 1 : vx + 1);
        }
        --vy;
    }
    return false;
}

// Same assumptions as in part one + that the target area is located to the right of the
// initial position (it is for the examples and my puzzle input).
// Simple solution, checks every possibility in:
//  - vx_range = [1, x_upper_bound] (any greater value would cause it to overshoot)
//  - vy_range = [y_lower_bound, max_possible_velocity] - second one is an answer from part one
unsigned number_of_possible_velocity_configurations(const Range& x_range, const Range& y_range) {
    if(!target_area_below_zero(y_range)) {
        throw std::runtime_error{"Invalid input - only negative y ranges supported."};
    }
    if(!target_area_to_the_right(x_range)) {
        throw std::runtime_error{"Invalid input - only positive x ranges supported."};
    }
    int max_init_vy = max_possible_initial_y_velocity(y_range);
    int min_init_vy = y_range.first;
    int max_init_vx = x_range.second;
    int min_init_vx = 1;
    unsigned number_of_configurations = 0;
    for(int vx = min_init_vx; vx <= max_init_vx; ++vx) {
        for(int vy = min_init_vy; vy <= max_init_vy; ++vy) {
            if(target_area_possible_to_reach(x_range, y_range, vx, vy)) {
                ++number_of_configurations;
            }
        }
    }
    return number_of_configurations;
}

int main() {
    const auto[x_range, y_range] = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << max_possible_height(y_range) << std::endl;
    std::cout << "Part 2: " << number_of_possible_velocity_configurations(x_range, y_range) << std::endl;
    return 0;
}