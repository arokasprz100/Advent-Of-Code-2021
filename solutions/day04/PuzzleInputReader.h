#pragma once

#include <PuzzleInput.h>

class PuzzleInputReader {
public:

    static PuzzleInput read(const std::string& file_name) {
        std::ifstream file{file_name};
        if(!file.is_open()) {
            throw std::runtime_error{"Could not open file " + file_name};
        }
        std::vector<unsigned> numbers = read_generated_numbers_from_file(file);
        std::vector<BingoBoard> boards = read_all_boards_from_file(file);
        return {numbers, boards};
    }

private:

    static std::vector<unsigned> read_generated_numbers_from_file(std::ifstream& file) {
        std::string numbers_line{};
        std::getline(file, numbers_line);
        return utils::comma_separated_string_to_numbers(numbers_line);
    }

    static std::optional<BingoBoard> read_board_from_file(std::ifstream& file) {
        std::array<unsigned, BINGO_BOARD_TILE_COUNT> board_content_row_major{};
        for(unsigned index = 0; index < BINGO_BOARD_TILE_COUNT; ++index) {
            if(!(file >> board_content_row_major.at(index))) {
                return std::nullopt;
            }
        }
        return BingoBoard{board_content_row_major};
    }

    static std::vector<BingoBoard> read_all_boards_from_file(std::ifstream& file) {
        std::vector<BingoBoard> boards{};
        while(true) {
            if(auto board = read_board_from_file(file); board) {
                boards.push_back(board.value());
            } else break;
        }
        return boards;
    }
};