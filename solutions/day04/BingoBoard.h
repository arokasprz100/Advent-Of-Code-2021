#pragma once

constexpr unsigned BINGO_BOARD_SIDE{5};
constexpr unsigned BINGO_BOARD_TILE_COUNT{BINGO_BOARD_SIDE * BINGO_BOARD_SIDE};

class BingoBoard {
public:

    explicit BingoBoard(const std::array<unsigned, BINGO_BOARD_TILE_COUNT>& content) :
        content_row_major{content} {}

    void mark(unsigned value_to_mark) {
        for(unsigned index = 0; index < BINGO_BOARD_TILE_COUNT; ++index) {
            if(content_row_major.at(index) == value_to_mark) {
                marked_fields.at(index) = true;
            }
        }
    }

    [[nodiscard]]
    unsigned score(unsigned last_value) const {
        unsigned unmarked_sum = 0;
        for(unsigned index = 0; index < BINGO_BOARD_TILE_COUNT; ++index) {
            if(!marked_fields.at(index)) {
                unmarked_sum += content_row_major.at(index);
            }
        }
        return unmarked_sum * last_value;
    }

    [[nodiscard]]
    bool is_winner() const {
        return is_any_column_marked() || is_any_row_marked();
    }

private:
    std::array<unsigned, BINGO_BOARD_TILE_COUNT> content_row_major{};
    std::array<bool, BINGO_BOARD_TILE_COUNT> marked_fields{};

    [[nodiscard]]
    bool is_any_column_marked() const {
        for(unsigned column_number = 0; column_number < BINGO_BOARD_SIDE; ++column_number) {
            if(is_column_marked(column_number)) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]]
    bool is_any_row_marked() const {
        for(unsigned row_number = 0; row_number < BINGO_BOARD_SIDE; ++row_number) {
            if(is_row_marked(row_number)) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]]
    bool is_row_marked(unsigned row_number) const {
        for(unsigned index_in_board = row_number * BINGO_BOARD_SIDE; index_in_board < (row_number + 1) * BINGO_BOARD_SIDE; ++index_in_board) {
            if(!marked_fields.at(index_in_board)) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]]
    bool is_column_marked(unsigned column_number) const {
        for(unsigned index_in_board = column_number; index_in_board < BINGO_BOARD_TILE_COUNT; index_in_board += BINGO_BOARD_SIDE) {
            if(!marked_fields.at(index_in_board)) {
                return false;
            }
        }
        return true;
    }
};