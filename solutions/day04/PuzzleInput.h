#pragma once

#include <BingoBoard.h>

struct PuzzleInput {
    std::vector<unsigned> generated_numbers{};
    std::vector<BingoBoard> boards{};
};
