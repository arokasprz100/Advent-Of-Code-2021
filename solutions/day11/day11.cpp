#include <iostream>
#include <fstream>
#include <array>
#include <set>

#include <Utils.h>

constexpr unsigned GRID_SIDE{10};
using DumboOctopusesGrid = std::array<std::array<unsigned, GRID_SIDE>, GRID_SIDE>;
using Octopus = std::pair<unsigned, unsigned>;

DumboOctopusesGrid read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    DumboOctopusesGrid puzzle_input{};
    for(unsigned i = 0; i < GRID_SIDE; ++i) {
        std::string input_line{};
        std::getline(file, input_line);
        std::stringstream input_line_stream{utils::trim(input_line)};
        for(unsigned j = 0; j < GRID_SIDE; ++j) {
            char input_character{};
            input_line_stream >> input_character;
            puzzle_input.at(i).at(j) = static_cast<unsigned>(input_character - '0');
        }
    }
    return puzzle_input;
}

bool has_octopus_flashed(const Octopus& octopus, const std::set<Octopus>& octopuses_that_flashed) {
    return octopuses_that_flashed.find(octopus) != std::end(octopuses_that_flashed);
}

void increase_energy_level_of_each_octopus_by_one(DumboOctopusesGrid& octopuses_grid) {
    for(unsigned i = 0; i < GRID_SIDE; ++i) {
        for(unsigned j = 0; j < GRID_SIDE; ++j) {
            ++octopuses_grid.at(i).at(j);
        }
    }
}

void set_energy_level_of_octopuses_that_flashed_to_zero(DumboOctopusesGrid& octopuses_grid, const std::set<Octopus>& octopuses_that_flashed) {
    for(const auto& octopus: octopuses_that_flashed) {
        octopuses_grid.at(octopus.first).at(octopus.second) = 0;
    }
}

void flash_if_energy_level_high_enough(const Octopus& to_check, DumboOctopusesGrid& octopuses_grid, std::set<Octopus>& octopuses_that_flashed) {
    if(has_octopus_flashed(to_check, octopuses_that_flashed) || octopuses_grid.at(to_check.first).at(to_check.second) <= 9) {
        return;
    }
    for(int i = -1; i <= 1; ++i) {
        for(int j = -1; j <= 1; ++j) {
            if(i == 0 && j == 0) continue;
            if(static_cast<int>(to_check.first) + i < 0 || to_check.first + i >= GRID_SIDE ||
               static_cast<int>(to_check.second) + j < 0 || to_check.second + j >= GRID_SIDE) continue;
            ++octopuses_grid.at(to_check.first + i).at(to_check.second + j);
        }
    }
    octopuses_that_flashed.insert(to_check);
}

void iterate_over_colony_until_no_more_flashes(DumboOctopusesGrid& octopuses_grid, std::set<Octopus>& octopuses_that_flashed) {
    while(true) {
        unsigned number_of_flashes_before_iteration = octopuses_that_flashed.size();
        for(unsigned i = 0; i < GRID_SIDE; ++i) {
            for(unsigned j = 0; j < GRID_SIDE; ++j) {
                flash_if_energy_level_high_enough({i, j}, octopuses_grid, octopuses_that_flashed);
            }
        }
        if(number_of_flashes_before_iteration == octopuses_that_flashed.size()) {
            break;
        }
    }
}

std::pair<DumboOctopusesGrid, unsigned> model_single_step(DumboOctopusesGrid octopuses_grid) {
    std::set<Octopus> octopuses_that_flashed{};
    increase_energy_level_of_each_octopus_by_one(octopuses_grid);
    iterate_over_colony_until_no_more_flashes(octopuses_grid, octopuses_that_flashed);
    set_energy_level_of_octopuses_that_flashed_to_zero(octopuses_grid, octopuses_that_flashed);
    return {
        octopuses_grid,
        octopuses_that_flashed.size()
    };
}

unsigned model_colony_of_octopuses_for_given_number_of_steps(const DumboOctopusesGrid& initial_state, unsigned number_of_steps) {
    unsigned total_number_of_flashes{0};
    DumboOctopusesGrid octopuses_colony = initial_state;
    for(unsigned step = 0; step < number_of_steps; ++step) {
        const auto[octopuses_colony_after_step, number_of_flashes_in_a_step] = model_single_step(octopuses_colony);
        octopuses_colony = octopuses_colony_after_step;
        total_number_of_flashes += number_of_flashes_in_a_step;
    }
    return total_number_of_flashes;
}

unsigned modeL_colony_of_octopuses_until_all_octopuses_flash_simultaneously(const DumboOctopusesGrid& initial_state) {
    DumboOctopusesGrid octopuses_colony = initial_state;
    unsigned number_of_flashes_in_last_step{0};
    unsigned step_number{0};
    while(number_of_flashes_in_last_step != GRID_SIDE * GRID_SIDE) {
        ++step_number;
        const auto[octopuses_colony_after_step, number_of_flashes_in_a_step] = model_single_step(octopuses_colony);
        octopuses_colony = octopuses_colony_after_step;
        number_of_flashes_in_last_step = number_of_flashes_in_a_step;
    }
    return step_number;
}

int main() {
    const auto puzzle_input = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << model_colony_of_octopuses_for_given_number_of_steps(puzzle_input, 100) << std::endl;
    std::cout << "Part 2: " << modeL_colony_of_octopuses_until_all_octopuses_flash_simultaneously(puzzle_input) << std::endl;
    return 0;
}