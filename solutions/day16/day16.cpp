#include <iostream>
#include <fstream>
#include <numeric>
#include <Utils.h>
#include <Converters.h>
#include <PacketsParser.h>

std::string read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    std::string puzzle_input{};
    file >> puzzle_input;
    return utils::trim(puzzle_input);
}

unsigned solve_part_one(const std::string& hex_transmission) {
    std::string binary_transmission = converters::hex_to_binary(hex_transmission);
    const auto packet = PacketsParser::parse(binary_transmission);
    return packet->get_version_sum();
}

long long solve_part_two(const std::string& hex_transmission) {
    std::string binary_transmission = converters::hex_to_binary(hex_transmission);
    const auto packet = PacketsParser::parse(binary_transmission);
    return packet->get_value();
}

int main() {
    const auto puzzle_input = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << solve_part_one(puzzle_input) << std::endl;
    std::cout << "Part 2: " << solve_part_two(puzzle_input) << std::endl;
    return 0;
}