#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <optional>
#include <Utils.h>

class SnailFishNumber {
public:

    SnailFishNumber(std::vector<unsigned> elements, std::vector<unsigned> nesting_levels) :
        elements{std::move(elements)},
        nesting_levels{std::move(nesting_levels)} {}

    SnailFishNumber operator+(const SnailFishNumber& rhs) const {
        std::vector<unsigned> elements_of_the_result{};
        elements_of_the_result.reserve(elements.size() + rhs.elements.size());
        elements_of_the_result.insert(std::end(elements_of_the_result), std::begin(elements), std::end(elements));
        elements_of_the_result.insert(std::end(elements_of_the_result), std::begin(rhs.elements), std::end(rhs.elements));

        std::vector<unsigned> nesting_levels_of_the_result{};
        nesting_levels_of_the_result.reserve(nesting_levels.size() + rhs.nesting_levels.size());
        nesting_levels_of_the_result.insert(std::end(nesting_levels_of_the_result), std::begin(nesting_levels), std::end(nesting_levels));
        nesting_levels_of_the_result.insert(std::end(nesting_levels_of_the_result), std::begin(rhs.nesting_levels), std::end(rhs.nesting_levels));
        std::for_each(std::begin(nesting_levels_of_the_result), std::end(nesting_levels_of_the_result), [](unsigned& level) { ++level; });

        SnailFishNumber result{elements_of_the_result, nesting_levels_of_the_result};
        result.reduce();
        return result;
    }

    [[nodiscard]]
    unsigned magnitude() const {
        std::vector<unsigned> magnitudes = elements;
        std::vector<unsigned> depths = nesting_levels;
        unsigned deepest_level = *(std::max_element(std::begin(depths), std::end(depths)));
        while(magnitudes.size() > 1) {
            std::vector<unsigned> to_remove{};
            for(unsigned i = 0; i < magnitudes.size(); ++i) {
                if(depths.at(i) == deepest_level) {
                    magnitudes.at(i) = 3 * magnitudes.at(i) + 2 * magnitudes.at(i + 1);
                    --depths.at(i);
                    to_remove.push_back(i + 1);
                    ++i;
                }
            }
            for(unsigned i = 0; i < to_remove.size(); ++i) {
                magnitudes.erase(std::begin(magnitudes) + to_remove.at(i) - i);
                depths.erase(std::begin(depths) + to_remove.at(i) - i);
            }
            --deepest_level;
        }
        return magnitudes.empty() ? 0 : magnitudes.at(0);
    }

private:
    std::vector<unsigned> elements{};
    std::vector<unsigned> nesting_levels{};

    [[nodiscard]]
    std::optional<std::pair<unsigned, unsigned>> pair_to_explode_indexes() const {
        for(unsigned i = 0; i < nesting_levels.size() - 1; ++i) {
            if(nesting_levels.at(i) == 5 && nesting_levels.at(i + 1) == 5) {
                return std::optional<std::pair<unsigned, unsigned>>{std::in_place, i, i + 1};
            }
        }
        return std::nullopt;
    }

    [[nodiscard]]
    std::optional<unsigned> regular_number_to_split_index() const {
        for(unsigned i = 0; i < elements.size(); ++i) {
            if(elements.at(i) >= 10) return i;
        }
        return std::nullopt;
    }

    [[nodiscard]]
    bool explode_if_possible() {
        if(auto pair_to_explode = pair_to_explode_indexes(); pair_to_explode.has_value()) {
            auto[first, second] = pair_to_explode.value();
            auto[first_value, second_value] = std::pair<unsigned, unsigned>{elements.at(first), elements.at(second)};
            elements.at(first) = 0;
            --nesting_levels.at(first);
            if(first != 0) {
                elements.at(first - 1) += first_value;
            }
            if(second < elements.size() - 1) {
                elements.at(second + 1) += second_value;
            }
            elements.erase(std::begin(elements) + second);
            nesting_levels.erase(std::begin(nesting_levels) + second);
            return true;
        }
        return false;
    }

    [[nodiscard]]
    bool split_if_possible() {
        if(auto number_to_split = regular_number_to_split_index(); regular_number_to_split_index().has_value()) {
            auto index = number_to_split.value();
            auto rounded_down = static_cast<unsigned>(std::floor(elements.at(index) / 2.0));
            auto rounded_up = static_cast<unsigned>(std::ceil(elements.at(index) / 2.0));
            elements.insert(std::begin(elements) + index, rounded_down);
            elements.at(index + 1) = rounded_up;
            nesting_levels.insert(std::begin(nesting_levels) + index, nesting_levels.at(index) + 1);
            ++nesting_levels.at(index + 1);
            return true;
        }
        return false;
    }

    void reduce() {
        while(true) {
            bool explode_result = explode_if_possible();
            if(!explode_result) {
                bool split_result = split_if_possible();
                if(!split_result) {
                    break;
                }
            }
        }
    }

};

SnailFishNumber parse_snail_fish_number(const std::string& string_representation) {
    unsigned current_depth = 0;
    std::string current_number{};
    std::vector<unsigned> elements{};
    std::vector<unsigned> nesting_levels{};
    for(char character: string_representation) {
        if(character == '[') {
            ++current_depth;
        }
        else if(character == ']') {
            if(!current_number.empty()) {
                unsigned number = std::stoul(current_number);
                current_number.clear();
                elements.push_back(number);
                nesting_levels.push_back(current_depth);
            }
            --current_depth;
        }
        else if(character == ',') {
            if(!current_number.empty()) {
                unsigned number = std::stoul(current_number);
                current_number.clear();
                elements.push_back(number);
                nesting_levels.push_back(current_depth);
            }
        }
        else {
            current_number += character;
        }
    }
    return SnailFishNumber{elements, nesting_levels};
}

std::vector<SnailFishNumber> read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    std::vector<SnailFishNumber> puzzle_input{};
    for(std::string input_line; std::getline(file, input_line); ) {
        input_line = utils::trim(input_line);
        puzzle_input.emplace_back(parse_snail_fish_number(input_line));
    }
    return puzzle_input;
}

unsigned solve_part_one(const std::vector<SnailFishNumber>& puzzle_input) {
    SnailFishNumber addition_result = puzzle_input.at(0);
    for(unsigned i = 1; i < puzzle_input.size(); ++i) {
        addition_result = addition_result + puzzle_input.at(i);
    }
    return addition_result.magnitude();
}

unsigned solve_part_two(const std::vector<SnailFishNumber>& puzzle_input) {
    unsigned highest_magnitude = 0;
    for(unsigned i = 0; i < puzzle_input.size(); ++i) {
        for(unsigned j = 0; j < puzzle_input.size(); ++j) {
            if(i == j) continue;
            auto result = puzzle_input.at(i) + puzzle_input.at(j);
            unsigned magnitude = result.magnitude();
            highest_magnitude = magnitude > highest_magnitude ? magnitude : highest_magnitude;
        }
    }
    return highest_magnitude;
}

int main() {
    const auto puzzle_input = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << solve_part_one(puzzle_input) << std::endl;
    std::cout << "Part 2: " << solve_part_two(puzzle_input) << std::endl;
    return 0;
}