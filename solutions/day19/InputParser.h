#pragma once

#include <fstream>
#include <Utils.h>
#include <Scanner.h>

namespace input_parser {

    Point3D parse_beacon_position(const std::string& input_line) {
        std::vector<std::string> coordinate_strings = utils::split_by_delimiter(input_line, ",");
        return Point3D{ std::stoi(coordinate_strings.at(0)), std::stoi(coordinate_strings.at(1)), std::stoi(coordinate_strings.at(2))};
    }

    Scanner parse_scanner(std::ifstream& file, unsigned current_scanner) {
        Scanner scanner{current_scanner, {}};
        for(std::string input_line{}; std::getline(file, input_line); ) {
            input_line = utils::trim(input_line);
            if(input_line.empty()) {
                break;
            }
            else {
                scanner.beacons.emplace_back(parse_beacon_position(input_line));
            }
        }
        return scanner;
    }

    std::vector<Scanner> read_puzzle_input(const std::string& file_name) {
        std::ifstream file{file_name};
        if(!file.is_open()) {
            throw std::runtime_error{"Could not open file " + file_name};
        }
        std::vector<Scanner> scanners{};
        unsigned current_scanner = 0;
        for(std::string input_line{}; std::getline(file, input_line); ) {
            input_line = utils::trim(input_line);
            if(input_line.starts_with("--- scanner ")) {
                Scanner scanner = parse_scanner(file, current_scanner++);
                scanners.emplace_back(scanner);
            }
        }
        return scanners;
    }
}