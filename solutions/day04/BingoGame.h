#pragma once

#include <BingoBoard.h>

class BingoGame {
public:

    BingoGame(std::vector<BingoBoard> boards, std::function<unsigned()> numbers_generator) :
            bingo_boards{std::move(boards)},
            numbers_generator{std::move(numbers_generator)} {}

    unsigned get_first_winning_score() {
        std::optional<unsigned> winning_score = std::nullopt;
        while(!winning_score) {
            winning_score = next_turn();
        }
        return winning_score.value();
    }

    unsigned get_last_winning_score() {
        while(bingo_boards.size() > 1) {
            get_first_winning_score();
            remove_winner_boards();
        }
        if(bingo_boards.empty()) {
            throw std::runtime_error{"Could not decide single last winning board"};
        }
        return get_first_winning_score();
    }

private:
    std::vector<BingoBoard> bingo_boards{};
    std::function<unsigned()> numbers_generator{};

    std::optional<unsigned> next_turn() {
        unsigned next_value = numbers_generator();
        update_boards(next_value);
        return get_highest_winning_score(next_value);
    }

    void update_boards(unsigned next_value) {
        for(auto& board: bingo_boards) {
            board.mark(next_value);
        }
    }

    [[nodiscard]]
    std::optional<unsigned> get_highest_winning_score(unsigned last_value) {
        std::optional<unsigned> highest_winning_score{};
        for(const auto& board: bingo_boards) {
            if(board.is_winner()) {
                auto score = board.score(last_value);
                highest_winning_score = highest_winning_score.value_or(0) <= score ? score : highest_winning_score;
            }
        }
        return highest_winning_score;
    }

    void remove_winner_boards() {
        auto winner_predicate = [](const auto& board) { return board.is_winner(); };
        bingo_boards.erase(
            std::remove_if(std::begin(bingo_boards), std::end(bingo_boards), winner_predicate),
            std::end(bingo_boards));
    }

};