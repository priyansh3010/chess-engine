#pragma once
#include "types.h"
#include "move.h"

struct MoveInfo {
    Move move;
    int prevEnPassantSquare;
    int prevCastling;
    int prevHalfMoveClock;
    int prevFullMoveNumber;
};