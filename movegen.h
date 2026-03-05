#pragma once
#include <iostream>
#include <vector>
#include "board.h"
#include "move.h"
#include "types.h"
using namespace std;

namespace MoveGen {

    vector<U64> knightAttacks(1ULL, 64);

    void init();
    vector<Move> generateLegalMoves(const Board& board);
}