#pragma once
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include "board.h"
#include "evaluation.h"
#include "move.h"
#include "moveinfo.h"
#include "movegen.h"
#include "types.h"
#include "utils.h"
using namespace std;

const int INF = 1000000;
const int MAX_DEPTH = 20;
Move movePool[256 * (MAX_DEPTH)]; // +1 for the getBestMove level

// for time management
auto searchStartTime = chrono::steady_clock::now();
int searchAllocatedMs = 5000;
bool stopSearch = false;
int nodesSearched = 0;

namespace {
    int minimax(Board& board, int depth, int alpha, int beta, Move* movePool, int plyFromRoot, int& nodesSearched) {
        bool maximizingPlayer = board.sideToMove == WHITE;
        // check if search time is over
        if ((nodesSearched & 2047) == 0 && utils::isTimeUp(searchAllocatedMs, searchStartTime)) stopSearch = true;

        // terminate minimax if search over
        if (stopSearch) return 0;
        
        // increment nodesSearched
        nodesSearched++;

        // evaluate final position
        if (depth == 0) return Evaluation::evaluate(board);

        Move* moves = movePool + (plyFromRoot * 256);
        int moveCount = 0;
        MoveGen::generateLegalMoves(board, moves, moveCount); // generate legal opponent moves

        // check if game is stalemated or checkmated
        if (moveCount == 0) {
            // checkmated
            if (board.isKingInCheck(board.sideToMove)) {
                return board.sideToMove == WHITE ? -INF + plyFromRoot : INF - plyFromRoot;
            }
            // stalemated
            return 0;
        }

        // if whites turn
        if (maximizingPlayer) {
            int bestEval = -INF;
            for (int i = 0; i < moveCount; i++) {
                Move& move = moves[i];
                MoveInfo moveInfo = board.makeMove(move);
                int currEval = minimax(board, depth - 1, alpha, beta, movePool, plyFromRoot + 1, nodesSearched);
                board.unMakeMove(moveInfo);
                bestEval = max(bestEval, currEval);
                
                // alpha-beta pruning
                alpha = max(alpha, currEval);
                if (beta <= alpha) break;
            }
            return bestEval;
        }
        // if blacks turn
        else {
            int bestEval = INF;
            for (int i = 0; i < moveCount; i++) {
                Move& move = moves[i];
                MoveInfo moveInfo = board.makeMove(move);
                int currEval = minimax(board, depth - 1, alpha, beta, movePool, plyFromRoot + 1, nodesSearched);
                board.unMakeMove(moveInfo);
                bestEval = min(bestEval, currEval);

                // beta pruning
                beta = min(beta, currEval);
                if (beta <= alpha) break;
            }
            return bestEval;
        }
    }

    Move searchRoot(Board& board, int depth, Move* moves, int& moveCount, int& nodesSearched) {
        bool maximizing = board.sideToMove == WHITE;

        Move bestMove = moves[0];
        int bestEval = maximizing ? INT_MIN : INT_MAX;
        for (int i = 0; i < moveCount; i++) {
            Move& move = moves[i];
            MoveInfo moveInfo = board.makeMove(move);
            int currEval = minimax(board, depth - 1, -INF, INF, movePool, 1, nodesSearched);
            board.unMakeMove(moveInfo);

            if (maximizing) {
                if (bestEval < currEval) {
                    bestEval = currEval;
                    bestMove = move;
                } 
            }
            else {
                if (bestEval > currEval) {
                    bestEval = currEval;
                    bestMove = move;
                }     
            }
        }
        
        return bestMove;
    }
}

namespace Engine {
    Move getBestMove(Board& board, int allocatedMS) {
        Move* moves = movePool;
        int moveCount = 0;
        MoveGen::generateLegalMoves(board, moves, moveCount);

        searchStartTime = chrono::steady_clock::now();
        searchAllocatedMs = allocatedMS;
        stopSearch = false;
        nodesSearched = 0;

        Move bestMove = moveCount != 0 ? moves[0] : Move{};
        int maxDepthReached = 1;
        for (int depth = 1; depth <= MAX_DEPTH; depth++) {
            Move candidateMove = searchRoot(board, depth, moves, moveCount, nodesSearched);

            if (stopSearch) break;

            maxDepthReached = depth;
            bestMove = candidateMove;
        }
        
        cout << "info depth " << maxDepthReached << " score cp " << Evaluation::evaluate(board) << " nodes " << nodesSearched << endl;
        
        return bestMove;
    }
}