#pragma once

namespace converters {

    std::string hex_to_binary(const char hex_digit, unsigned output_width = 4) {
        unsigned value = hex_digit >= 'A' ? hex_digit - 'A' + 10 : hex_digit - '0';
        std::string binary_string{};
        for(unsigned i = 0; i < output_width; ++i) {
            binary_string.insert(0, std::to_string((value >> i) & 1));
        }
        return binary_string;
    }

    std::string hex_to_binary(const std::string& hex_string) {
        std::string binary_string{};
        for(const auto& character: hex_string) {
            binary_string += hex_to_binary(character);
        }
        return binary_string;
    }

    inline unsigned binary_to_decimal(const std::string& binary) {
        return std::stoul(binary, nullptr, 2);
    }

    inline long long binary_to_long_decimal(const std::string& binary) {
        return static_cast<long long>(std::stoull(binary, nullptr, 2));
    }

}
