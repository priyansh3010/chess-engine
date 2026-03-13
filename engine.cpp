#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "board.h"
#include "move.h"
#include "moveinfo.h"
#include "movegen.h"
#include "types.h"

namespace Engine {
    Move getBestMove(Board& board) {
        vector<Move> legalMoves = MoveGen::generateLegalMoves(board);

        return legalMoves[rand() % legalMoves.size()];
    }
}