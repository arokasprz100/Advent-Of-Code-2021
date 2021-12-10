#include <utility>
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

#include <Utils.h>
#include <Geometry.h>
#include <Map.h>

using LineCheck = std::function<bool(const Line&)>;

Point string_to_point(const std::string& point_text) {
    auto values = utils::split_by_delimiter(point_text, ",");
    return std::make_pair(std::stol(values.at(0)), std::stol(values.at(1)));
}

std::vector<Line> read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    std::string current_line{};
    std::vector<Line> lines{};
    while(std::getline(file, current_line)) {
        auto points = utils::split_by_delimiter(current_line, "->");
        auto start = string_to_point(points.at(0));
        auto end = string_to_point(points.at(1));
        lines.emplace_back(start, end);
    }
    return lines;
}

unsigned get_number_of_points_where_lines_overlap(const std::vector<Line>& lines, const LineCheck& line_acceptance_predicate) {
    Map map{1000, 1000};
    for(const auto& line: lines) {
        if(line_acceptance_predicate(line)) {
            const auto points_in_line = line.get_points();
            for(const auto& point: points_in_line) {
                map.increase_occupation_count(point);
            }
        }
    }
    return map.count_points_where_lines_overlap();
}

unsigned solve_part_one(const std::vector<Line>& lines) {
    auto line_vertical_or_horizontal = [](const Line& line) {
        return line.is_vertical() || line.is_horizontal();
    };
    return get_number_of_points_where_lines_overlap(lines, line_vertical_or_horizontal);
}

unsigned solve_part_two(const std::vector<Line>& lines) {
    auto line_vertical_horizontal_or_diagonal = [](const Line& line) {
        return line.is_vertical() || line.is_horizontal() || line.is_diagonal();
    };
    return get_number_of_points_where_lines_overlap(lines, line_vertical_horizontal_or_diagonal);
}

int main() {
    auto puzzle_input = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << solve_part_one(puzzle_input) << std::endl;
    std::cout << "Part 2: " << solve_part_two(puzzle_input) << std::endl;
    return 0;
}