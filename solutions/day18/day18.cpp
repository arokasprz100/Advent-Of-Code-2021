#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <numeric>
#include <Utils.h>

struct RegularNumber {
    unsigned value = 0;
    unsigned depth = 0;
};

struct RegularNumberDepthComparator {
    bool operator()(const RegularNumber& lhs, const RegularNumber& rhs) {
        return lhs.depth < rhs.depth;
    }
};

class SnailFishNumber {
public:

    explicit SnailFishNumber(std::vector<RegularNumber> elements) :
        elements{std::move(elements)} {}

    SnailFishNumber operator+(const SnailFishNumber& rhs) const {
        std::vector<RegularNumber> elements_of_the_result{};
        elements_of_the_result.reserve(elements.size() + rhs.elements.size());
        elements_of_the_result.insert(std::end(elements_of_the_result), std::begin(elements), std::end(elements));
        elements_of_the_result.insert(std::end(elements_of_the_result), std::begin(rhs.elements), std::end(rhs.elements));
        std::for_each(std::begin(elements_of_the_result), std::end(elements_of_the_result), [](RegularNumber& element) { ++(element.depth); });
        SnailFishNumber result{elements_of_the_result};
        result.reduce();
        return result;
    }

    [[nodiscard]]
    unsigned magnitude() const {
        static RegularNumberDepthComparator depth_comparator{};
        std::vector<RegularNumber> magnitudes_with_depths = elements;
        unsigned deepest_level = std::max_element(std::begin(elements), std::end(elements), depth_comparator)->depth;
        while(magnitudes_with_depths.size() > 1) {
            unsigned number_of_magnitudes = magnitudes_with_depths.size();
            unsigned number_of_removed = 0;
            for(unsigned i = 0; i < number_of_magnitudes - number_of_removed; ++i) {
                if(magnitudes_with_depths.at(i).depth == deepest_level) {
                    RegularNumber new_magnitude_with_depth{
                        3 * magnitudes_with_depths.at(i).value + 2 * magnitudes_with_depths.at(i + 1).value,
                        magnitudes_with_depths.at(i).depth - 1
                    };
                    magnitudes_with_depths.at(i) = new_magnitude_with_depth;
                    magnitudes_with_depths.erase(std::begin(magnitudes_with_depths) + i + 1);
                    ++number_of_removed;
                }
            }
            --deepest_level;
        }
        return magnitudes_with_depths.empty() ? 0 : magnitudes_with_depths.at(0).value;
    }

private:
    std::vector<RegularNumber> elements{};

    [[nodiscard]]
    bool explode_if_possible() {
        for(unsigned index = 0; index < elements.size() - 1; ++index) {
            if(elements.at(index).depth != 5) {
                continue;
            }
            RegularNumber first_number = elements.at(index);
            RegularNumber second_number = elements.at(index + 1);
            elements.at(index) = {0, first_number.depth - 1};
            if(index != 0) {
                elements.at(index - 1).value += first_number.value;
            }
            if(index + 1 < elements.size() - 1) {
                elements.at(index + 2).value += second_number.value;
            }
            elements.erase(std::begin(elements) + index + 1);
            return true;
        }
        return false;
    }

    [[nodiscard]]
    bool split_if_possible() {
        for(unsigned index = 0; index < elements.size(); ++index) {
            if(elements.at(index).value < 10) {
                continue;
            }
            RegularNumber to_split = elements.at(index);
            RegularNumber rounded_down{static_cast<unsigned>(std::floor(to_split.value / 2.0)), to_split.depth + 1};
            RegularNumber rounded_up{static_cast<unsigned>(std::ceil(to_split.value / 2.0)), to_split.depth + 1};
            elements.insert(std::begin(elements) + index, rounded_down);
            elements.at(index + 1) = rounded_up;
            return true;
        }
        return false;
    }

    void reduce() {
        for(bool reduced = true; reduced; ) {
            reduced = explode_if_possible();
            if(!reduced) {
                reduced = split_if_possible();
            }
        }
    }
};

SnailFishNumber parse_snail_fish_number(const std::string& string_representation) {
    unsigned current_depth = 0;
    std::string current_number{};
    std::vector<RegularNumber> elements{};
    for(char character: string_representation) {
        if(character == '[') {
            ++current_depth;
        }
        else if(character == ']') {
            if(!current_number.empty()) {
                unsigned number = std::stoul(current_number);
                current_number.clear();
                elements.emplace_back(RegularNumber{number, current_depth});
            }
            --current_depth;
        }
        else if(character == ',') {
            if(!current_number.empty()) {
                unsigned number = std::stoul(current_number);
                current_number.clear();
                elements.emplace_back(RegularNumber{number, current_depth});
            }
        }
        else {
            current_number += character;
        }
    }
    return SnailFishNumber{elements};
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
    auto addition_result = std::accumulate(std::begin(puzzle_input) + 1, std::end(puzzle_input), puzzle_input.front());
    return addition_result.magnitude();
}

unsigned solve_part_two(const std::vector<SnailFishNumber>& puzzle_input) {
    unsigned highest_magnitude = 0;
    for(unsigned i = 0; i < puzzle_input.size() - 1; ++i) {
        for(unsigned j = i + 1; j < puzzle_input.size(); ++j) {
            unsigned magnitude_in_order = (puzzle_input.at(i) + puzzle_input.at(j)).magnitude();
            unsigned magnitude_reversed = (puzzle_input.at(j) + puzzle_input.at(i)).magnitude();
            highest_magnitude = std::max(highest_magnitude, std::max(magnitude_in_order, magnitude_reversed));
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