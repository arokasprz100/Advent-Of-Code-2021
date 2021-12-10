#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <optional>
#include <numeric>
#include <cmath>

#include <Utils.h>

std::vector<std::string> read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    std::vector<std::string> puzzle_input{};
    for(std::string input_line{}; std::getline(file, input_line); ) {
        puzzle_input.emplace_back(utils::trim(input_line));
    }
    return puzzle_input;
}


bool is_chunk_opening(char to_check) {
    return to_check == '(' || to_check == '[' || to_check == '{' || to_check == '<';
}

char get_chunk_closing_for_opening(char opening) {
    static const std::map<char, char> chunk_closings_by_openings {
        { '(', ')' },
        { '[', ']' },
        { '{', '}' },
        { '<', '>' }
    };
    return chunk_closings_by_openings.at(opening);
}

bool is_chunk_closing_correct(char closing, char last_opening) {
    return get_chunk_closing_for_opening(last_opening) == closing;
}

unsigned get_points_for_illegal_character(char illegal_character) {
    static const std::map<char, unsigned> points_by_characters {
        { ')', 3 },
        { ']', 57 },
        { '}', 1197 },
        { '>', 25137 },
    };
    return points_by_characters.at(illegal_character);
}

std::optional<char> find_first_illegal_character(const std::string& line) {
    std::stack<char> chunk_openings{};
    for(const char character: line) {
        if(is_chunk_opening(character)) {
            chunk_openings.push(character);
        }
        else {
            if(chunk_openings.empty()) {
                return character;
            }
            char last_opening = chunk_openings.top();
            chunk_openings.pop();
            if(!is_chunk_closing_correct(character, last_opening)) {
                return character;
            }
        }
    }
    return std::nullopt;
}


std::vector<std::string> remove_corrupted_lines(std::vector<std::string> navigation_subsystem) {
    auto line_corrupted_predicate = [](const std::string& line) {
        return find_first_illegal_character(line).has_value();
    };
    navigation_subsystem.erase(
        std::remove_if(std::begin(navigation_subsystem), std::end(navigation_subsystem), line_corrupted_predicate),
        std::end(navigation_subsystem));
    return navigation_subsystem;
}

std::string determine_completion_string(const std::string& line) {
    std::stack<char> chunk_openings{};
    for(const char character: line) {
        if(is_chunk_opening(character)) {
            chunk_openings.push(character);
        }
        else {
            chunk_openings.pop();
        }
    }
    std::string completion_string{};
    while(!chunk_openings.empty()) {
        char last_opening = chunk_openings.top();
        chunk_openings.pop();
        completion_string += get_chunk_closing_for_opening(last_opening);
    }
    return completion_string;
}

long long int get_points_for_completion_string(const std::string& completion_string) {
    static const std::map<char, long long int> points_by_character {
        { ')', 1ll },
        { ']', 2ll },
        { '}', 3ll },
        { '>', 4ll },
    };
    static const auto points_accumulator = [](long long int previous_sum, char next_character) {
        return previous_sum * 5ll + points_by_character.at(next_character);
    };
    return std::accumulate(std::begin(completion_string), std::end(completion_string), 0ll, points_accumulator);
}

std::vector<long long int> get_points_for_incomplete_lines(const std::vector<std::string>& incomplete_lines) {
    const auto score_calculator = [](const auto& line) {
        return get_points_for_completion_string(determine_completion_string(line));
    };
    std::vector<long long int> scores{};
    std::transform(std::begin(incomplete_lines), std::end(incomplete_lines),
                   std::back_inserter(scores), score_calculator);
    return scores;
}


unsigned solve_part_one(const std::vector<std::string>& navigation_subsystem) {
    const auto points_accumulator = [](const unsigned previous_sum, const std::string& current_line) {
        auto illegal_character = find_first_illegal_character(current_line);
        return previous_sum + (illegal_character ? get_points_for_illegal_character(illegal_character.value()) : 0);
    };
    return std::accumulate(std::begin(navigation_subsystem), std::end(navigation_subsystem), 0, points_accumulator);
}

unsigned solve_part_two(const std::vector<std::string>& navigation_subsystem) {
    const auto incomplete_lines = remove_corrupted_lines(navigation_subsystem);
    auto scores = get_points_for_incomplete_lines(incomplete_lines);
    std::sort(std::begin(scores), std::end(scores));
    return scores.at(static_cast<unsigned>(std::floor(static_cast<double>(scores.size()) / 2.0)));
}


int main() {
    const auto puzzle_input = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << solve_part_one(puzzle_input) << std::endl;
    std::cout << "Part 2: " << solve_part_two(puzzle_input) << std::endl;
    return 0;
}