#pragma once
#include "types.h"
#include "move.h"

struct MoveInfo {
    Move move;
    int prevEnPassantSquare;
    int prevCastling;
    int prevHalfMoveClock;
    int prevFullMoveNumber;
    Color prevColor;

    MoveInfo(Move m, int prevEnPassant, int prevCastling, int prevHalfMove, 
         int prevFullMove, Color color)
        : move(m), prevEnPassantSquare(prevEnPassant), prevCastling(prevCastling),
          prevHalfMoveClock(prevHalfMove), prevFullMoveNumber(prevFullMove), prevColor(color) {}
};