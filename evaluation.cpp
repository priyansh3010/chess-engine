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

static const int PST[6][64] = {
    // king middlegame 
    {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        20, 20,  0,  0,  0,  0, 20, 20,
        20, 30, 10,  0,  0, 10, 30, 20
    },
    // queens
    {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -5,  0,  5,  10,  10,  5,  0, -5,
        0,  0,  5,  10,  10,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    },
    // rooks
    {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        0,  0,  0,  5,  5,  0,  0,  0
    },
    // bishops
    {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    },
    // knights
    {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    },
    // pawns
    {
        0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 25, 25, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-40,-40, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
    },
};

namespace {
    void countMaterial(Board& board, int& currEval) {
        // simple evaluation function to count all pieces

        // go through all the white pieces
        U64 whitePieces = board.occupancy[WHITE];
        while (whitePieces) {
            int currIndex = getLSB(whitePieces);
            Piece currPiece = board.getPieceAt(WHITE, currIndex);
            if (currPiece == QUEEN) currEval += 900;
            if (currPiece == ROOK) currEval += 500;
            if (currPiece == BISHOP) currEval += 300;
            if (currPiece == KNIGHT) currEval += 300;
            if (currPiece == PAWN) currEval += 100;

            whitePieces &= whitePieces - 1;
        }

        // go through all the black pieces
        U64 blackPieces = board.occupancy[BLACK];
        while (blackPieces) {
            int currIndex = getLSB(blackPieces);
            Piece currPiece = board.getPieceAt(BLACK, currIndex);

            // add all piece values
            if (currPiece == QUEEN) currEval -= 900;
            if (currPiece == ROOK) currEval -= 500;
            if (currPiece == BISHOP) currEval -= 300;
            if (currPiece == KNIGHT) currEval -= 300;
            if (currPiece == PAWN) currEval -= 100;

            blackPieces &= blackPieces - 1;
        }
    }

    void calculatePieceSquareTables(Board& board, int& currEval) {
        // loop through all pawns
        U64 whitePieces = board.pieces[WHITE][PAWN];
        U64 blackPieces = board.pieces[BLACK][PAWN];
        
        // loop through all white pawns
        while (whitePieces) {
            int currSquare = getLSB(whitePieces); // get curr pawn
            currEval += PST[PAWN][currSquare]; // add to eval
            whitePieces &= whitePieces - 1; // move to next LSB
        }
        // loop through all black pawns
        while (blackPieces) {
            int currSquare = getLSB(blackPieces); // get curr pawn
            currEval -= PST[PAWN][currSquare ^ 56]; // add to eval
            blackPieces &= blackPieces - 1; // move to next LSB
        }

        // loop through all knights
        whitePieces = board.pieces[WHITE][KNIGHT];
        blackPieces = board.pieces[BLACK][KNIGHT];

        // loop through all white knights
        while (whitePieces) {
            int currSquare = getLSB(whitePieces); // get curr knight
            currEval += PST[KNIGHT][currSquare]; // add to eval
            whitePieces &= whitePieces - 1; // move to next LSB
        }
        // loop through all black knights
        while (blackPieces) {
            int currSquare = getLSB(blackPieces); // get curr knight
            currEval -= PST[KNIGHT][currSquare ^ 56]; // add to eval
            blackPieces &= blackPieces - 1; // move to next LSB
        }

        // loop through all bishops
        whitePieces = board.pieces[WHITE][BISHOP];
        blackPieces = board.pieces[BLACK][BISHOP];

        // loop through all white bishops
        while (whitePieces) {
            int currSquare = getLSB(whitePieces); // get curr bishops
            currEval += PST[BISHOP][currSquare]; // add to eval
            whitePieces &= whitePieces - 1; // move to next LSB
        }
        // loop through all black bishops
        while (blackPieces) {
            int currSquare = getLSB(blackPieces); // get curr bishops
            currEval -= PST[BISHOP][currSquare ^ 56]; // add to eval
            blackPieces &= blackPieces - 1; // move to next LSB
        }

        // loop through all rooks
        whitePieces = board.pieces[WHITE][ROOK];
        blackPieces = board.pieces[BLACK][ROOK];

        // loop through all white rooks
        while (whitePieces) {
            int currSquare = getLSB(whitePieces); // get curr rooks
            currEval += PST[ROOK][currSquare]; // add to eval
            whitePieces &= whitePieces - 1; // move to next LSB
        }
        // loop through all black rooks
        while (blackPieces) {
            int currSquare = getLSB(blackPieces); // get curr rooks
            currEval -= PST[ROOK][currSquare ^ 56]; // add to eval
            blackPieces &= blackPieces - 1; // move to next LSB
        }

        // loop through all queens
        whitePieces = board.pieces[WHITE][QUEEN];
        blackPieces = board.pieces[BLACK][QUEEN];

        // loop through all white queens
        while (whitePieces) {
            int currSquare = getLSB(whitePieces); // get curr queens
            currEval += PST[QUEEN][currSquare]; // add to eval
            whitePieces &= whitePieces - 1; // move to next LSB
        }
        // loop through all black queens
        while (blackPieces) {
            int currSquare = getLSB(blackPieces); // get curr queens
            currEval -= PST[QUEEN][currSquare ^ 56]; // add to eval
            blackPieces &= blackPieces - 1; // move to next LSB
        }

        // check kings
        whitePieces = board.pieces[WHITE][KING];
        blackPieces = board.pieces[BLACK][KING];

        // check white king
        while (whitePieces) {
            int currSquare = getLSB(whitePieces); // get curr king
            currEval += PST[KING][currSquare]; // add to eval
            whitePieces &= whitePieces - 1; // move to next LSB
        }
        // check black king
        while (blackPieces) {
            int currSquare = getLSB(blackPieces); // get curr king
            currEval -= PST[KING][currSquare ^ 56]; // add to eval
            blackPieces &= blackPieces - 1; // move to next LSB
        }
    }
}

namespace Evaluation {
    int evaluate(Board& board) {
        int currEval = 0;
        
        countMaterial(board, currEval);
        calculatePieceSquareTables(board, currEval);
        
        return currEval;
    }
}