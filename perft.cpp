#pragma once
#include <string>
#include <vector>
#include "board.h"
#include "move.h"
#include "moveinfo.h"
#include "movegen.h"
#include "types.h"
#include "utils.h"
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
        uint64_t count = perft(board, depth - 1); 
        board.unMakeMove(moveInfo);

        cout << utils::moveToString(move);
        cout << ": " << count << endl;
        total += count;
    }
    cout << "\nTotal: " << total << endl;
    return total;
}