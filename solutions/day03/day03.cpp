#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <functional>

constexpr int BITSET_SIZE{12};
using DiagnosticReport = std::vector<std::bitset<BITSET_SIZE>>;
using BitCriteria = std::function<bool(unsigned, unsigned)>;

DiagnosticReport read_input_file(const std::string& file_path) {
    std::ifstream input_file{file_path};
    if(!input_file.is_open()) {
        throw std::runtime_error{"Could not open input file: " + file_path};
    }
    DiagnosticReport puzzle_input{};
    std::string current_binary_number{};
    while(input_file >> current_binary_number) {
        puzzle_input.emplace_back(current_binary_number);
    }
    return puzzle_input;
}

std::pair<unsigned, unsigned> count_zeros_and_ones_in_a_column(const DiagnosticReport& diagnostic_report, unsigned column_number) {
    std::pair<unsigned, unsigned> number_of_zeros_and_ones{0, 0};
    for(const auto& report_row: diagnostic_report) {
        if(report_row.test(column_number)) {
            number_of_zeros_and_ones.second += 1;
        }
        else {
            number_of_zeros_and_ones.first += 1;
        }
    }
    return number_of_zeros_and_ones;
}

DiagnosticReport remove_entries_with_non_matching_value(const DiagnosticReport& diagnostic_report, unsigned column, bool value) {
    DiagnosticReport new_report{};
    std::copy_if(std::begin(diagnostic_report), std::end(diagnostic_report),
                 std::back_inserter(new_report),
                 [&column, &value](const std::bitset<BITSET_SIZE>& report_row) { return report_row.test(column) == value; });
    return new_report;
}

std::bitset<BITSET_SIZE> apply_bit_criteria_to_get_rating(DiagnosticReport diagnostic_report, const BitCriteria& bit_criteria) {
    for(unsigned column_number = BITSET_SIZE - 1; column_number >= 0; --column_number) {
        if(diagnostic_report.size() <= 1) {
            break;
        }
        const auto [number_of_zeros, number_of_ones] = count_zeros_and_ones_in_a_column(diagnostic_report, column_number);
        bool value_to_keep = bit_criteria(number_of_zeros, number_of_ones);
        diagnostic_report = remove_entries_with_non_matching_value(diagnostic_report, column_number, value_to_keep);
    }
    return diagnostic_report.at(0);
}


unsigned long solve_part_one(const DiagnosticReport& diagnostic_report) {
    std::bitset<BITSET_SIZE> gamma_rate{};
    for(unsigned column_number = 0; column_number < BITSET_SIZE; ++column_number) {
        const auto [number_of_zeros, number_of_ones] = count_zeros_and_ones_in_a_column(diagnostic_report, column_number);
        gamma_rate.set(column_number, number_of_ones > number_of_zeros);
    }
    std::bitset<BITSET_SIZE> epsilon_rate = ~gamma_rate;
    return epsilon_rate.to_ulong() * gamma_rate.to_ulong();
}

unsigned long solve_part_two(const DiagnosticReport& diagnostic_report) {
    const BitCriteria oxygen_generator_rating_bit_criteria = [](unsigned number_of_zeros, unsigned number_of_ones) {
        return number_of_zeros <= number_of_ones; // 1 if more or equal amount of ones, 0 otherwise
    };
    const auto oxygen_generator_rating = apply_bit_criteria_to_get_rating(diagnostic_report, oxygen_generator_rating_bit_criteria);
    const BitCriteria co2_scrubber_rating_bit_criteria = [](unsigned number_of_zeros, unsigned number_of_ones) {
        return number_of_zeros > number_of_ones; // 1 if less amount of ones, 0 otherwise
    };
    const auto co2_scrubber_rating = apply_bit_criteria_to_get_rating(diagnostic_report, co2_scrubber_rating_bit_criteria);
    return oxygen_generator_rating.to_ulong() * co2_scrubber_rating.to_ulong();
}


int main() {
    const auto puzzle_input = read_input_file("input.txt");
    std::cout << "Part 1: " << solve_part_one(puzzle_input) << std::endl;
    std::cout << "Part 2: " << solve_part_two(puzzle_input) << std::endl;
    return 0;
}
