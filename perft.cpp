#pragma once
#include <string>
#include <vector>
#include "board.h"
#include "move.h"
#include "moveinfo.h"
#include "movegen.h"
#include "types.h"
using namespace std;


U64 perft(Board& board, int depth) {
    if (depth == 0) return 1;

    std::vector<Move> moves = MoveGen::generateLegalMoves(board);

    U64 nodes = 0;
    for (const Move& move : moves) {
        MoveInfo moveInfo = board.makeMove(move);
        nodes += perft(board, depth - 1);
        board.unMakeMove(moveInfo);
    }
    return nodes;
}

int perftDivide(Board& board, int depth) {
    vector<Move> moves = MoveGen::generateLegalMoves(board);
    
    char files[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
    int ranks[] = {1, 2, 3, 4, 5, 6, 7, 8};

    uint64_t total = 0;
    for (auto& move : moves) {
        MoveInfo moveInfo = board.makeMove(move);
        uint64_t count = perft(board, depth - 1);  // depth - 1, not depth
        board.unMakeMove(moveInfo);

        cout << files[move.fromSquare % 8] << ranks[move.fromSquare / 8]
        << files[move.toSquare % 8] << ranks[move.toSquare / 8];
        if (move.promotionPiece != NONE) {
            if (move.promotionPiece == QUEEN) cout << "q";
            else if (move.promotionPiece == ROOK) cout << "r";
            else if (move.promotionPiece == BISHOP) cout << "b";
            else if (move.promotionPiece == KNIGHT) cout << "n";
        }
        cout << ": " << count << endl;
        total += count;
    }
    cout << "\nTotal: " << total << endl;
    return total;
}