#pragma once
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include "board.h"
#include "move.h"
#include "moveinfo.h"
#include "movegen.h"
#include "types.h"
using namespace std;

namespace utils {
    string moveToString(Move move);
    Move stringToMove(Board& board, string moveStr);
    bool isTimeUp(int& searchAllocatedMS, std::chrono::steady_clock::time_point searchStartTime);
}