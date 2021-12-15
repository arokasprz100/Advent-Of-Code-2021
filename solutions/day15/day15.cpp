#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <queue>

#include <Utils.h>

using ChitonGrid = std::vector<std::vector<unsigned>>;
using Chiton = std::pair<unsigned, unsigned>;

ChitonGrid read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    ChitonGrid puzzle_input{};
    for(std::string input_line{}; std::getline(file, input_line); ) {
        std::vector<unsigned> row{};
        input_line = utils::trim(input_line);
        for(const auto& input_character: input_line) {
            row.emplace_back(static_cast<unsigned>(input_character - '0'));
        }
        puzzle_input.emplace_back(row);
    }
    return puzzle_input;
}

ChitonGrid initialize_costs_array(const ChitonGrid& risks) {
    ChitonGrid costs_array(risks.size(), std::vector<unsigned>(risks.at(0).size(), std::numeric_limits<unsigned>::max()));
    return costs_array;
}

std::vector<Chiton> get_neighbours(const Chiton& coordinates, const ChitonGrid& risks) {
    std::vector<Chiton> neighbours{};
    if(coordinates.first > 0) {
        neighbours.emplace_back(coordinates.first - 1, coordinates.second);
    }
    if(coordinates.first < risks.at(coordinates.second).size() - 1) {
        neighbours.emplace_back(coordinates.first + 1, coordinates.second);
    }
    if(coordinates.second > 0) {
        neighbours.emplace_back(coordinates.first, coordinates.second - 1);
    }
    if(coordinates.second < risks.size() - 1) {
        neighbours.emplace_back(coordinates.first, coordinates.second + 1);
    }
    return neighbours;
}

unsigned find_shortest_path_from_source_to_destination(const Chiton& source, const Chiton& destination, const ChitonGrid& risks) {
    ChitonGrid costs_array = initialize_costs_array(risks);
    costs_array.at(source.second).at(source.first) = 0;
    const auto cost_comparator = [&costs_array](const Chiton& lhs, const Chiton& rhs) {
        return costs_array.at(lhs.second).at(lhs.first) > costs_array.at(rhs.second).at(rhs.first);
    };
    std::priority_queue<Chiton, std::vector<Chiton>, decltype(cost_comparator)> queue(cost_comparator);
    queue.push(source);

    while(!queue.empty()) {
        Chiton next = queue.top();
        queue.pop();
        if(next == destination) {
            break;
        }
        const auto neighbours = get_neighbours(next, risks);
        for(const auto& neighbour: neighbours) {
            unsigned new_cost = costs_array.at(next.second).at(next.first) + risks.at(neighbour.second).at(neighbour.first);
            if(new_cost < costs_array.at(neighbour.second).at(neighbour.first)) {
                costs_array.at(neighbour.second).at(neighbour.first) = new_cost;
                queue.push(neighbour);
            }
        }
    }
    return costs_array.at(destination.second).at(destination.first);
}

unsigned solve_part_one(const ChitonGrid& risks) {
    Chiton source{0, 0};
    Chiton destination{risks.at(risks.size() - 1).size() - 1, risks.size() - 1};
    return find_shortest_path_from_source_to_destination(source, destination, risks);
}

ChitonGrid expand_grid(const ChitonGrid& initial_grid) {
    ChitonGrid resulting_grid(initial_grid.size() * 5, std::vector<unsigned>(initial_grid.at(0).size() * 5, 0));
    for(unsigned y = 0; y < initial_grid.size(); ++y) {
        for(unsigned x = 0; x < initial_grid.at(y).size(); ++x) {
            for(unsigned i = 0; i < 5; ++i) {
                for(unsigned j = 0; j < 5; ++j) {
                    unsigned increased_value = initial_grid.at(y).at(x) + i + j;
                    unsigned new_value = increased_value > 9 ? 1 + (increased_value % 10) : increased_value;
                    resulting_grid.at(initial_grid.size() * i + y).at(initial_grid.at(y).size() * j + x) = new_value;
                }
            }
        }
    }
    return resulting_grid;
}

unsigned solve_part_two(const ChitonGrid& risks) {
    const auto expanded_risks = expand_grid(risks);
    Chiton source{0, 0};
    Chiton destination{expanded_risks.at(expanded_risks.size() - 1).size() - 1, expanded_risks.size() - 1};
    return find_shortest_path_from_source_to_destination(source, destination, expanded_risks);
}

int main() {
    const auto puzzle_input = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << solve_part_one(puzzle_input) << std::endl;
    std::cout << "Part 2: " << solve_part_two(puzzle_input) << std::endl;
    return 0;
}