#include <iostream>
#include <fstream>
#include <regex>
#include <Utils.h>

struct Player {
    unsigned position = 0;
    unsigned score = 0;
};

unsigned read_player_position(const std::string& input_line) {
    std::regex player_position_regex{"Player ([0-9]+) starting position: ([0-9]+)"};
    std::smatch matches{};
    if(std::regex_search(input_line, matches, player_position_regex)) {
        return std::stoul(matches[2].str());
    }
    throw std::runtime_error{"Could not file player position in the string"};
}

std::pair<unsigned, unsigned> read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    std::vector<std::string> input_lines{};
    for(std::string input_line{}; std::getline(file, input_line); ) {
        input_lines.emplace_back(utils::trim(input_line));
    }
    std::pair<unsigned, unsigned> player_positions{};
    player_positions.first = read_player_position(input_lines.at(0));
    player_positions.second = read_player_position(input_lines.at(1));
    return player_positions;
}

template<typename Dice>
unsigned roll_dice_three_times(Dice& dice) {
    return dice() + dice() + dice();
}

unsigned move_forward(unsigned initial_position, unsigned distance) {
    return (initial_position + distance - 1) % 10 + 1;
}

template<typename Dice>
unsigned players_turn(unsigned position, Dice& dice) {
    unsigned dice_roll_result = roll_dice_three_times(dice);
    return move_forward(position, dice_roll_result);
}

unsigned solve_part_one(unsigned first_player_position, unsigned second_player_position) {
    auto deterministic_dice = [next = 1]() mutable {
        int current = next;
        next = (next % 100 + 1);
        return current;
    };
    std::vector<Player> players {{first_player_position, 0}, {second_player_position, 0}};
    unsigned current_player_index = 0;
    unsigned number_of_dice_rolls = 0;
    while(players.at(0).score < 1000 && players.at(1).score < 1000) {
        Player& current_player = players.at(current_player_index);
        current_player.position = players_turn(current_player.position, deterministic_dice);
        current_player.score += current_player.position;
        number_of_dice_rolls += 3;
        current_player_index = (current_player_index + 1) % 2;
    }
    unsigned smaller_score = std::min(players.at(0).score, players.at(1).score);
    return number_of_dice_rolls * smaller_score;
}

int main() {
    const auto[first_player_position, second_player_position] = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << solve_part_one(first_player_position, second_player_position) << std::endl;
    return 0;
}