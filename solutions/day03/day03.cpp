#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <array>

constexpr int BITSET_SIZE{12};

template<int NUMBER_OF_POSITIONS>
std::vector<std::bitset<NUMBER_OF_POSITIONS>> read_input_file(const std::string& file_path) {
    std::ifstream input_file{file_path};
    if(!input_file.is_open()) {
        throw std::runtime_error{"Could not open input file: " + file_path};
    }
    std::vector<std::bitset<NUMBER_OF_POSITIONS>> puzzle_input{};
    std::string current_binary_number{};
    while(input_file >> current_binary_number) {
        puzzle_input.emplace_back(current_binary_number);
    }
    return puzzle_input;
}

unsigned long solve_part_one(const std::vector<std::bitset<BITSET_SIZE>>& puzzle_input) {
    std::array<int, BITSET_SIZE> ones_and_zeros_count_differences{};
    for(const auto& binary_number: puzzle_input) {
        for(int i = 0; i < BITSET_SIZE; ++i) {
            ones_and_zeros_count_differences[i] += binary_number[i] ? 1 : -1;
        }
    }
    std::bitset<BITSET_SIZE> gamma_rate{};
    for(int i = 0; i < BITSET_SIZE; ++i) {
        gamma_rate.set(i, ones_and_zeros_count_differences[i] > 0);
    }
    std::bitset<BITSET_SIZE> epsilon_rate = ~gamma_rate;
    return epsilon_rate.to_ulong() * gamma_rate.to_ulong();
}

int main() {
    const auto puzzle_input = read_input_file<BITSET_SIZE>("input.txt");
    std::cout << "Part 1: " << solve_part_one(puzzle_input) << std::endl;
    return 0;
}
