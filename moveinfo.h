#pragma once
#include "types.h"

struct MoveInfo {
    Square fromSquare;
    Square toSquare;
    int prevEnPassantSquare;
    int prevCastling;
    int prevHalfMoveClock;
    int prevFullMoveNumber;
};