#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <memory>

#include "SubmarineSystems.h"

std::vector<DiveCommand> read_input_file(const std::string& file_path) {
    std::ifstream input_file{file_path};
    if(!input_file.is_open()) {
        throw std::runtime_error{"Could not open input file: " + file_path};
    }
    std::string direction{};
    int distance{};
    std::vector<DiveCommand> puzzle_input{};
    while(input_file >> direction >> distance) {
        puzzle_input.push_back({direction, distance});
    }
    return puzzle_input;
}

int process_commands(const std::vector<DiveCommand>& puzzle_input, std::unique_ptr<Submarine> submarine) {
    for(const auto& command: puzzle_input) {
        if(command.direction == "up") submarine->up(command.value);
        else if(command.direction == "down") submarine->down(command.value);
        else if(command.direction == "forward") submarine->forward(command.value);
        else throw std::runtime_error{"Unknown command: " + command.direction};
    }
    return submarine->get_depth() * submarine->get_horizontal_position();
}

int main() {
    const auto puzzle_input = read_input_file("input.txt");
    std::cout << "Part 1: " << process_commands(puzzle_input, std::make_unique<SimpleSubmarine>()) << std::endl;
    std::cout << "Part 2: " << process_commands(puzzle_input, std::make_unique<SubmarineWithAim>()) << std::endl;
    return 0;
}
