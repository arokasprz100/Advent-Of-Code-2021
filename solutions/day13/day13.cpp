#include <iostream>
#include <fstream>
#include <vector>
#include <set>

#include <Utils.h>

enum class FoldDirection {
    X, Y
};

using Dot = std::pair<unsigned, unsigned>;
using Fold = std::pair<FoldDirection, unsigned>;

bool is_dot_description(const std::string& input_line) {
    return input_line.find(',') != std::string::npos;
}

bool is_fold_instruction(const std::string& input_line) {
    return input_line.starts_with("fold along ");
}

Dot parse_dot_description(const std::string& input_line) {
    const auto parts = utils::split_by_delimiter(utils::trim(input_line), ",");
    return { std::stoul(parts.at(0)), std::stoul(parts.at(1)) };
}

Fold parse_fold_description(const std::string& input_line) {
    const auto parts = utils::split_by_delimiter(utils::trim(input_line), " ");
    const auto description_parts = utils::split_by_delimiter(parts.at(2), "=");
    const FoldDirection fold_direction = description_parts.at(0) == "x" ? FoldDirection::X : FoldDirection::Y;
    return { fold_direction, std::stoul(description_parts.at(1)) };
}

std::pair<std::set<Dot>, std::vector<Fold>> read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    std::set<Dot> dots_initial_positions{};
    std::vector<Fold> fold_instructions{};
    for(std::string input_line{}; std::getline(file, input_line); ) {
        if(is_dot_description(input_line)) {
            const auto dot = parse_dot_description(input_line);
            dots_initial_positions.emplace(dot);
        }
        else if(is_fold_instruction(input_line)) {
            const auto fold = parse_fold_description(input_line);
            fold_instructions.emplace_back(fold);
        }
    }
    return { dots_initial_positions, fold_instructions };
}

std::set<Dot> fold_using_vertical_line(const std::set<Dot>& dots, unsigned axis) {
    std::set<Dot> dots_after_reflection{};
    for(const auto& dot: dots) {
        if(dot.first < axis) {
            dots_after_reflection.emplace(dot);
        }
        else if(dot.first > axis) {
            unsigned difference = dot.first - axis;
            dots_after_reflection.emplace(axis - difference, dot.second);
        }
    }
    return dots_after_reflection;
}

std::set<Dot> fold_using_horizontal_line(const std::set<Dot>& dots, unsigned axis) {
    std::set<Dot> dots_after_reflection{};
    for(const auto& dot: dots) {
        if(dot.second < axis) {
            dots_after_reflection.emplace(dot);
        }
        else if(dot.second > axis) {
            unsigned difference = dot.second - axis;
            dots_after_reflection.emplace(dot.first, axis - difference);
        }
    }
    return dots_after_reflection;
}

std::set<Dot> perform_single_fold_instruction(const std::set<Dot>& dots, const Fold& fold_instruction) {
    if(fold_instruction.first == FoldDirection::Y) {
        return fold_using_horizontal_line(dots, fold_instruction.second);
    }
    else {
        return fold_using_vertical_line(dots, fold_instruction.second);
    }
}

std::set<Dot> perform_fold_instructions(const std::set<Dot>& dots_initial_positions, const std::vector<Fold>& fold_instructions) {
    std::set<Dot> dots_positions = dots_initial_positions;
    for(const auto& fold_instruction: fold_instructions) {
        dots_positions = perform_single_fold_instruction(dots_positions, fold_instruction);
    }
    return dots_positions;
}

unsigned max_x_coordinate(const std::set<Dot>& dots) {
    const auto comparator = [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; };
    return std::max_element(std::begin(dots), std::end(dots), comparator)->first;
}

unsigned max_y_coordinate(const std::set<Dot>& dots) {
    const auto comparator = [](const auto& lhs, const auto& rhs) { return lhs.second < rhs.second; };
    return std::max_element(std::begin(dots), std::end(dots), comparator)->second;
}

std::string dots_positions_to_string(const std::set<Dot>& dots) {
    unsigned max_x = max_x_coordinate(dots);
    unsigned max_y = max_y_coordinate(dots);
    std::vector<std::string> rows(max_y + 1, std::string(max_x + 1, '.'));
    for(const auto& dot: dots) {
        rows.at(dot.second).at(dot.first) = '#';
    }
    std::string result{"\n"};
    for(const auto& row: rows) {
        result += row + "\n";
    }
    return result;
}

unsigned solve_part_one(const std::set<Dot>& dots_initial_positions, const std::vector<Fold>& fold_instructions) {
    const auto instruction_to_perform = *fold_instructions.begin();
    return perform_single_fold_instruction(dots_initial_positions, instruction_to_perform).size();
}

std::string solve_part_two(const std::set<Dot>& dots_initial_positions, const std::vector<Fold>& fold_instructions) {
    const auto dots_after_folding = perform_fold_instructions(dots_initial_positions, fold_instructions);
    return dots_positions_to_string(dots_after_folding);
}

int main() {
    const auto[dots_initial_positions, fold_instructions] = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << solve_part_one(dots_initial_positions, fold_instructions) << std::endl;
    std::cout << "Part 2: " << solve_part_two(dots_initial_positions, fold_instructions) << std::endl;
    return 0;
}