#include <array>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>

#include <Utils.h>
#include <BingoGame.h>
#include <PuzzleInput.h>
#include <PuzzleInputReader.h>


unsigned solve_part_one(const PuzzleInput& puzzle_input) {
    auto numbers_generator = [numbers = puzzle_input.generated_numbers, current = 0]() mutable {
        return numbers.at(current++);
    };
    BingoGame bingo_game{puzzle_input.boards, numbers_generator};
    return bingo_game.get_first_winning_score();
}

unsigned solve_part_two(const PuzzleInput& puzzle_input) {
    auto numbers_generator = [numbers = puzzle_input.generated_numbers, current = 0]() mutable {
        return numbers.at(current++);
    };
    BingoGame bingo_game{puzzle_input.boards, numbers_generator};
    return bingo_game.get_last_winning_score();
}


int main() {
    auto puzzle_input = PuzzleInputReader::read("input.txt");
    std::cout << "Part 1: " << solve_part_one(puzzle_input) << std::endl;
    std::cout << "Part 2: " << solve_part_two(puzzle_input) << std::endl;
    return 0;
}