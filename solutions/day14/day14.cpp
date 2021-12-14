#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <Utils.h>

using PairHistogram = std::map<std::string, long long int>;
using InsertionRules = std::map<std::string, std::string>;

std::pair<std::string, std::map<std::string, std::string>> read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    std::vector<std::string> puzzle_input{};
    for(std::string input_line{}; std::getline(file, input_line); ) {
        input_line = utils::trim(input_line);
        if(!input_line.empty()) {
            puzzle_input.emplace_back(utils::trim(input_line));
        }
    }
    std::string polymer_template = puzzle_input.front();
    std::map<std::string, std::string> pair_insertion_rules{};
    for(unsigned i = 1; i < puzzle_input.size(); ++i) {
        const auto parts = utils::split_by_delimiter(puzzle_input.at(i), "->");
        pair_insertion_rules[utils::trim(parts.at(0))] = utils::trim(parts.at(1));
    }
    return { polymer_template, pair_insertion_rules };
}

PairHistogram initialize_polymer_pair_histogram(const std::string& polymer_template) {
    PairHistogram polymer_pair_histogram{};
    for(unsigned i = 0; i < polymer_template.size() - 1; ++i) {
        std::string current_pair{ polymer_template.at(i), polymer_template.at(i + 1) };
        ++polymer_pair_histogram[current_pair];
    }
    return polymer_pair_histogram;
}

PairHistogram apply_insertion_rules_on_polymer_histogram(const PairHistogram& histogram, const InsertionRules& pair_insertion_rules) {
    PairHistogram new_pair_histogram = histogram;
    for(const auto& pair_entry: histogram) {
        long long int pair_count = pair_entry.second;
        std::string current_pair = pair_entry.first;
        const std::string& rule_for_pair = pair_insertion_rules.at(current_pair);
        new_pair_histogram[std::string{current_pair.at(0)} + rule_for_pair] += pair_count;
        new_pair_histogram[rule_for_pair + std::string{current_pair.at(1)}] += pair_count;
        new_pair_histogram[current_pair] -= pair_count;
    }
    return new_pair_histogram;
}

PairHistogram apply_insertion_rules_for_given_number_of_steps(const std::string& polymer_template, const InsertionRules& pair_insertion_rules, unsigned number_of_steps) {
    PairHistogram histogram = initialize_polymer_pair_histogram(polymer_template);
    for(unsigned i = 0; i < number_of_steps; ++i) {
        histogram = apply_insertion_rules_on_polymer_histogram(histogram, pair_insertion_rules);
    }
    return histogram;
}

std::map<char, long long int> initialize_elements_histogram(const PairHistogram& polymer_pair_histogram, const std::string& polymer_template) {
    std::map<char, long long int> counters{};
    for(const auto& histogram_entry: polymer_pair_histogram) {
        std::string current_pair = histogram_entry.first;
        counters[current_pair.at(0)] += histogram_entry.second;
        counters[current_pair.at(1)] += histogram_entry.second;
    }
    for(auto& element_counter: counters) {
        if(element_counter.first == polymer_template.front() || element_counter.first == polymer_template.back()) {
            element_counter.second = (element_counter.second - 1) / 2 + 1;
        }
        else {
            element_counter.second /= 2;
        }
    }
    return counters;
}

long long int most_and_least_common_elements_quantity_difference(const PairHistogram& polymer_pair_histogram, const std::string& polymer_template) {
    std::map<char, long long int> counters = initialize_elements_histogram(polymer_pair_histogram, polymer_template);
    const auto map_elements_value_comparator = [](const auto& lhs, const auto& rhs) {
        return lhs.second < rhs.second;
    };
    const auto [least_common, most_common] = std::minmax_element(std::begin(counters), std::end(counters), map_elements_value_comparator);
    return most_common->second - least_common->second;
}

long long int run_simulation(const std::string& polymer_template, const std::map<std::string, std::string>& pair_insertion_rules, unsigned number_of_steps) {
    const auto polymer_histogram = apply_insertion_rules_for_given_number_of_steps(polymer_template, pair_insertion_rules, number_of_steps);
    return most_and_least_common_elements_quantity_difference(polymer_histogram, polymer_template);
}

int main() {
    const auto[polymer_template, insertion_rules] = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << run_simulation(polymer_template, insertion_rules, 10) << std::endl;
    std::cout << "Part 2: " << run_simulation(polymer_template, insertion_rules, 40) << std::endl;
    return 0;
}