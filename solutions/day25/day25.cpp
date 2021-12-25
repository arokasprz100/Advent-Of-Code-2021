#include <iostream>
#include <fstream>
#include <vector>
#include <Utils.h>

enum class SeafloorState {
    EAST_MOVING_SEA_CUCUMBER,
    SOUTH_MOVING_SEA_CUCUMBER,
    NOTHING
};

class SeaCucumberHerds {
public:

    SeaCucumberHerds(std::vector<SeafloorState> seafloor, unsigned width, unsigned height)
        : seafloor_state{std::move(seafloor)}, seafloor_width{width}, seafloor_height{height} {
        if(seafloor_width * seafloor_height != seafloor_state.size()) {
            throw std::runtime_error{"Seafloor size does not match given width and height"};
        }
    }

    unsigned move_herds() {
        unsigned number_of_moved_cucumbers = 0;
        number_of_moved_cucumbers += east_moving_herd_step();
        number_of_moved_cucumbers += south_moving_herd_step();
        return number_of_moved_cucumbers;
    }

private:
    std::vector<SeafloorState> seafloor_state{};
    unsigned seafloor_width{};
    unsigned seafloor_height{};

    unsigned east_moving_herd_step() {
        const auto next_index_calculator = [width = seafloor_width](unsigned cucumber_index) {
            return ((cucumber_index + 1) % width == 0) ? cucumber_index + 1 - width : cucumber_index + 1;
        };
        return move_herd_one_step(SeafloorState::EAST_MOVING_SEA_CUCUMBER, next_index_calculator);
    }

    unsigned south_moving_herd_step() {
        const auto next_index_calculator = [height = seafloor_height, width = seafloor_width](unsigned cucumber_index) {
            return ((cucumber_index + width) < width * height) ? cucumber_index + width : (cucumber_index % width);
        };
        return move_herd_one_step(SeafloorState::SOUTH_MOVING_SEA_CUCUMBER, next_index_calculator);
    }

    template<typename IndexCalculator>
    unsigned move_herd_one_step(const SeafloorState& herd_type, const IndexCalculator& next_index_calculator) {
        unsigned number_of_moved_cucumbers = 0;
        std::vector<SeafloorState> new_floor_state = seafloor_state;
        for(unsigned i = 0; i < seafloor_state.size(); ++i) {
            if(seafloor_state.at(i) == herd_type) {
                unsigned next_index = next_index_calculator(i);
                if(seafloor_state.at(next_index) == SeafloorState::NOTHING) {
                    new_floor_state.at(i) = SeafloorState::NOTHING;
                    new_floor_state.at(next_index) = herd_type;
                    ++number_of_moved_cucumbers;
                }
            }
        }
        seafloor_state = new_floor_state;
        return number_of_moved_cucumbers;
    }
};

SeafloorState parse_seafloor_state(const char state) {
    switch (state) {
        case '>': return SeafloorState::EAST_MOVING_SEA_CUCUMBER;
        case 'v': return SeafloorState::SOUTH_MOVING_SEA_CUCUMBER;
        case '.': return SeafloorState::NOTHING;
        default: throw std::runtime_error{"Invalid input"};
    }
}

SeaCucumberHerds read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    std::vector<SeafloorState> seafloor{};
    unsigned seafloor_height = 0;
    for(std::string input_line{}; std::getline(file, input_line); ) {
        input_line = utils::trim(input_line);
        if(input_line.empty()) continue;
        for(const char character: input_line) {
            seafloor.emplace_back(parse_seafloor_state(character));
        }
        ++seafloor_height;
    }
    if(seafloor.size() % seafloor_height != 0) {
        throw std::runtime_error{"Invalid input"};
    }
    unsigned seafloor_width = seafloor.size() / seafloor_height;
    return {seafloor, seafloor_width, seafloor_height};
}

unsigned solve_part_one(SeaCucumberHerds herds) {
    unsigned number_of_steps = 0;
    while(herds.move_herds() != 0) {
        ++number_of_steps;
    }
    return number_of_steps + 1;
}

int main() {
    const auto puzzle_input = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << solve_part_one(puzzle_input) << std::endl;
    return 0;
}