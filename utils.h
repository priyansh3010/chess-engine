#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "board.h"
#include "move.h"
#include "moveinfo.h"
#include "movegen.h"
#include "types.h"

namespace utils {
    string moveToString(Move move);
    Move stringToMove(Board& board, string moveStr);
}