#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <map>
#include <unordered_set>
#include <cmath>

#include <Utils.h>

constexpr unsigned NUMBER_OF_UNIQUE_SIGNAL_PATTERNS{10};
constexpr unsigned NUMBER_OF_DIGITS{4};

using UniqueSignalPatterns = std::array<std::string, NUMBER_OF_UNIQUE_SIGNAL_PATTERNS>;
using FourDigitOutput = std::array<std::string, NUMBER_OF_DIGITS>;
using NotesEntry = std::pair<UniqueSignalPatterns, FourDigitOutput>;

std::vector<NotesEntry> read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    std::vector<NotesEntry> puzzle_input{};
    for(std::string input_line; std::getline(file, input_line); ) {
        std::stringstream input_line_stream{utils::trim(input_line)};
        UniqueSignalPatterns signal_patterns{};
        FourDigitOutput four_digits{};
        for(unsigned i = 0; i < NUMBER_OF_UNIQUE_SIGNAL_PATTERNS + NUMBER_OF_DIGITS + 1; ++i) {
            std::string entry{};
            input_line_stream >> entry;
            if(i < NUMBER_OF_UNIQUE_SIGNAL_PATTERNS) {
                signal_patterns.at(i) = entry;
            }
            else if(i > NUMBER_OF_UNIQUE_SIGNAL_PATTERNS) {
                four_digits.at(i - NUMBER_OF_UNIQUE_SIGNAL_PATTERNS - 1) = entry;
            }
        }
        puzzle_input.emplace_back(signal_patterns, four_digits);
    }
    return puzzle_input;
}

unsigned solve_part_one(const std::vector<NotesEntry>& notes) {
    const std::unordered_set<unsigned> desired_output_lengths{2, 4, 3, 7};
    unsigned counter{0};
    for(const auto& note_entry: notes) {
        for(const auto& output_digit: note_entry.second) {
            if(desired_output_lengths.find(output_digit.size()) != std::end(desired_output_lengths)) {
                ++counter;
            }
        }
    }
    return counter;
}

unsigned number_of_common_segments(const std::string& first_pattern, const std::string& second_pattern) {
    unsigned counter{0};
    for(const auto& segment: first_pattern) {
        if(second_pattern.find(segment) != std::string::npos) {
            ++counter;
        }
    }
    return counter;
}

unsigned get_digit_for_pattern_with_length_five(const std::string& pattern, const std::map<unsigned, std::string>& patterns_by_their_digits) {
    if(number_of_common_segments(pattern, patterns_by_their_digits.at(1)) == 2) {
        return 3; // unique property of pattern for 3: it has exactly two common segments with pattern for 1
    }
    else if(number_of_common_segments(pattern, patterns_by_their_digits.at(4)) == 2) {
        return 2; // unique property of pattern for 2: it has exactly two common segments with pattern for 4
    }
    else {
        return 5; // if above two properties are not true, pattern corresponds to 5
    }
}

unsigned get_digit_for_pattern_with_length_six(const std::string& pattern, const std::map<unsigned, std::string>& patterns_by_their_digits) {
    if(number_of_common_segments(pattern, patterns_by_their_digits.at(4)) == 4) {
        return 9; // unique property of pattern for 9: it has exactly four common segments with pattern for 4
    }
    else if(number_of_common_segments(pattern, patterns_by_their_digits.at(7)) == 2) {
        return 6; // unique property of pattern for 6: it has exactly two common segments with pattern for 7
    }
    else {
        return 0; // if above two properties are not true, pattern corresponds to 0
    }
}

unsigned get_digit_for_pattern(const std::string& pattern, const std::map<unsigned, std::string>& patterns_by_their_digits) {
    static const std::map<unsigned, unsigned> digits_by_unambiguous_pattern_lengths { { 2, 1 }, { 4, 4 }, { 3, 7 }, { 7, 8 } };
    if(pattern.size() == 5) { // gets 2, 3 and 5
        return get_digit_for_pattern_with_length_five(pattern, patterns_by_their_digits);
    }
    else if(pattern.size() == 6) { // gets 0, 6 and 9
        return get_digit_for_pattern_with_length_six(pattern, patterns_by_their_digits);
    }
    else { // gets 1, 4, 7, and 8
        return digits_by_unambiguous_pattern_lengths.at(pattern.size());
    }
}

unsigned four_digits_as_signal_patterns_to_number(const FourDigitOutput& signal_patterns, const std::map<unsigned, std::string>& patterns_by_their_digits) {
    unsigned number{0};
    for(int i = 0; i < 4; ++i) {
        std::string output_digit_pattern_sorted = signal_patterns.at(i);
        std::sort(std::begin(output_digit_pattern_sorted), std::end(output_digit_pattern_sorted));
        const auto second_in_pair_equal_to_pattern_predicate = [&output_digit_pattern_sorted](const auto& kv_pair) {
            return kv_pair.second == output_digit_pattern_sorted;
        };
        const auto found_digit_pattern_pair = std::find_if(std::begin(patterns_by_their_digits),
                                                           std::end(patterns_by_their_digits),
                                                           second_in_pair_equal_to_pattern_predicate);
        number += found_digit_pattern_pair->first * static_cast<unsigned>(std::pow(10, 3 - i));
    }
    return number;
}

UniqueSignalPatterns sort_signal_patterns_by_length(const UniqueSignalPatterns& unsorted_patterns) {
    const auto lengths_comparator = [](const auto& left, const auto& right) { return left.size() < right.size(); };
    UniqueSignalPatterns patterns_sorted_by_length = unsorted_patterns;
    std::sort(std::begin(patterns_sorted_by_length), std::end(patterns_sorted_by_length), lengths_comparator);
    return patterns_sorted_by_length;
}

unsigned solve_part_two(const std::vector<NotesEntry>& notes) {
    unsigned sum{0};
    for(const auto& entry: notes) {
        const auto patterns_sorted_by_length = sort_signal_patterns_by_length(entry.first); // important, without that algorithm will not work
        std::map<unsigned, std::string> patterns_by_their_digits{};
        for(std::string pattern: patterns_sorted_by_length) {
            std::sort(std::begin(pattern), std::end(pattern));
            unsigned digit_for_pattern = get_digit_for_pattern(pattern, patterns_by_their_digits);
            patterns_by_their_digits[digit_for_pattern] = pattern;
        }
        sum += four_digits_as_signal_patterns_to_number(entry.second, patterns_by_their_digits);
    }
    return sum;
}

int main() {
    const auto puzzle_input = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << solve_part_one(puzzle_input) << std::endl;
    std::cout << "Part 2: " << solve_part_two(puzzle_input) << std::endl;
    return 0;
}