#include <chrono>
#include <iostream>
#include <string>
#include "types.h"
#include "board.h"
#include "move.h"
#include "moveinfo.h"
#include "movegen.h"
#include "perft.h"
#include "uci.h"
using namespace std;

namespace {
    int countMaterial(Board& board) {
        // simple evaluation function to count all pieces
        int currEval = 0;

        // go through all the white pieces
        U64 whitePieces = board.occupancy[WHITE];
        while (whitePieces) {
            int currIndex = getLSB(whitePieces);
            Piece currPiece = board.getPieceAt(WHITE, currIndex);
            if (currPiece == QUEEN) currEval += 9;
            if (currPiece == ROOK) currEval += 5;
            if (currPiece == BISHOP) currEval += 3;
            if (currPiece == KNIGHT) currEval += 3;
            if (currPiece == PAWN) currEval += 1;

            whitePieces &= whitePieces - 1;
        }

        // go through all the black pieces
        U64 blackPieces = board.occupancy[BLACK];
        while (blackPieces) {
            int currIndex = getLSB(blackPieces);
            Piece currPiece = board.getPieceAt(BLACK, currIndex);

            // add all piece values
            if (currPiece == QUEEN) currEval -= 9;
            if (currPiece == ROOK) currEval -= 5;
            if (currPiece == BISHOP) currEval -= 3;
            if (currPiece == KNIGHT) currEval -= 3;
            if (currPiece == PAWN) currEval -= 1;

            blackPieces &= blackPieces - 1;
        }

        return currEval;
    }
}

namespace Evaluation {
    int evaluate(Board& board) {
        return countMaterial(board);
    }
}