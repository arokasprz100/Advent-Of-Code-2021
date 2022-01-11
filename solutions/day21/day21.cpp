#include <iostream>
#include <fstream>
#include <regex>
#include <Utils.h>

// TODO: refactor, improve performance !!!!!!!!!!!

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

template<typename Dice> // TODO: remove template
unsigned roll_dice_three_times(Dice& dice) {
    return dice() + dice() + dice();
}

inline unsigned move_forward(unsigned initial_position, unsigned distance) {
    return (initial_position + distance - 1) % 10 + 1;
}

template<typename Dice> // TODO: remove template
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

std::vector<unsigned> generate_triple_dice_roll_scores() {
    std::vector<unsigned> outcome{};
    for(unsigned i = 1; i <= 3; ++i) {
        for(unsigned j = 1; j <= 3; ++j) {
            for(unsigned k = 1; k <= 3; ++k) {
                outcome.emplace_back(i + j + k);
            }
        }
    }
    return outcome;
}

struct GameState {
    unsigned first_player_score = 0;
    unsigned first_player_position = 0;
    unsigned second_player_score = 0;
    unsigned second_player_position = 0;

    [[nodiscard]] unsigned get_score(unsigned index) const {
        return index == 0 ? first_player_score : second_player_score;
    }

    void set_score(unsigned index, unsigned value) {
        if(index == 0) {
            first_player_score = value;
        }
        else {
            second_player_score = value;
        }
    }

    [[nodiscard]] unsigned get_position(unsigned index) const {
        return index == 0 ? first_player_position : second_player_position;
    }

    void set_position(unsigned index, unsigned value) {
        if(index == 0) {
            first_player_position = value;
        }
        else {
            second_player_position = value;
        }
    }
};

bool operator==(const GameState& lhs, const GameState& rhs) {
    return lhs.first_player_score == rhs.first_player_score &&
           lhs.first_player_position == rhs.first_player_position &&
           lhs.second_player_score == rhs.second_player_score &&
           lhs.second_player_position == rhs.second_player_position;
}

struct GameStateHash {
    std::size_t operator()(const GameState& game_state) const {
        return game_state.first_player_score +
               20 * game_state.first_player_position +
               20 * 10 * game_state.second_player_score +
               20 * 10 * 20 * game_state.second_player_position; // TODO: check if good hash
    }
};

long long solve_part_two(unsigned first_player_position, unsigned second_player_position) {
    const auto possible_dice_roll_scores = generate_triple_dice_roll_scores();
    std::unordered_map<GameState, long long, GameStateHash> unfinished_games_histogram;
    unfinished_games_histogram[GameState{0, first_player_position, 0, second_player_position}] = 1;
    unsigned current_player = 0;
    std::unordered_map<GameState, long long, GameStateHash> new_histogram;
    std::vector<long long> games_won_by_player{0, 0};
    while(!unfinished_games_histogram.empty()) {
        new_histogram.clear();
        for(const auto&[game_state, count] : unfinished_games_histogram) {
            for(const auto& dice_roll_score : possible_dice_roll_scores) {
                unsigned new_position = move_forward(game_state.get_position(current_player), dice_roll_score);
                unsigned new_score = game_state.get_score(current_player) + new_position;
                if(new_score >= 21) {
                    games_won_by_player.at(current_player) += count;
                    continue;
                }
                GameState new_game_state = game_state;
                new_game_state.set_position(current_player, new_position);
                new_game_state.set_score(current_player, new_score);
                new_histogram[new_game_state] += count;
            }
        }
        current_player = (current_player + 1) % 2;
        unfinished_games_histogram = new_histogram;
    }
    return std::max(games_won_by_player.at(0), games_won_by_player.at(1));
}

int main() {
    const auto[first_player_position, second_player_position] = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << solve_part_one(first_player_position, second_player_position) << std::endl;
    std::cout << "Part 2: " << solve_part_two(first_player_position, second_player_position) << std::endl;
    return 0;
}