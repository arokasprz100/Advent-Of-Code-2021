#pragma once

#include <vector>
#include <string>
#include <sstream>

namespace utils {

// TODO: unit test, refactor
std::vector<std::string> split_by_delimiter(std::string to_split, const std::string& delimiter) {
    std::vector<std::string> tokens{};
    size_t delimiter_position{0};
    while((delimiter_position = to_split.find(delimiter)) != std::string::npos) {
        std::string token = to_split.substr(0, delimiter_position);
        to_split.erase(0, delimiter_position + delimiter.length());
        tokens.push_back(token);
    }
    if(!to_split.empty()) {
        tokens.push_back(to_split);
    }
    return tokens;
}

std::vector<unsigned> comma_separated_string_to_numbers(const std::string& to_convert) {
    auto separated_numbers_as_text = utils::split_by_delimiter(to_convert, ",");
    std::vector<unsigned> numbers{};
    std::transform(std::begin(separated_numbers_as_text), std::end(separated_numbers_as_text),
                   std::back_inserter(numbers),
                   [](const auto& number_text) { return std::stoul(number_text); });
    return numbers;
}

}