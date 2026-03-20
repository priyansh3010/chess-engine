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

// move ordering namespace
namespace {
    constexpr int mvvLvaValues[6] = {
        0, // king (shouldn't be captured but just in case)
        900, // queen
        500, // rook
        300, // bishop
        300, // knight
        100, // pawn
    };

    int scoreMVVLVA(const Move& move) {
        return mvvLvaValues[move.capturedPiece] - mvvLvaValues[move.pieceType];
    }

    void orderMoves(Move* moves, int moveCount, int bestMoveIdx = -1) {
        // assign scores
        static int scores[256];
        for (int i = 0; i < moveCount; i++) {
            if (i == bestMoveIdx) {
                scores[i] = 100000; // always search best move first
            } 
            else if (moves[i].capturedPiece != NONE) {
                scores[i] = scoreMVVLVA(moves[i]);
            } 
            else {
                scores[i] = 0;
            }
        }
        // simple insertion sort
        for (int i = 1; i < moveCount; i++) {
            int key = scores[i];
            Move m = moves[i];
            int j = i - 1;
            while (j >= 0 && scores[j] < key) {
                scores[j + 1] = scores[j];
                moves[j + 1] = moves[j];
                j--;
            }
            scores[j + 1] = key;
            moves[j + 1] = m;
        }
    }
}

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

        // move ordering
        orderMoves(moves, moveCount);

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

    int searchRoot(Board& board, int depth, Move* moves, int& moveCount, int& nodesSearched, int bestMove = -1) {
        bool maximizing = board.sideToMove == WHITE;

        int bestEval = maximizing ? -INF : INF;
        
        // move ordering
        orderMoves(moves, moveCount);
        
        // start from best move (if present)
        for (int i = 0; i < moveCount; i++) {
            Move& move = moves[i];
            MoveInfo moveInfo = board.makeMove(move);
            int currEval = minimax(board, depth - 1, -INF, INF, movePool, 1, nodesSearched);
            board.unMakeMove(moveInfo);

            if (maximizing) {
                if (bestEval < currEval) {
                    bestEval = currEval;
                    bestMove = i;
                } 
            }
            else {
                if (bestEval > currEval) {
                    bestEval = currEval;
                    bestMove = i;
                }     
            }
        }
        
        return bestMove;
    }
}

namespace Engine {
    Move getBestMove(Board& board, int allocatedMS) {
        // get all legal moves on the board
        Move* moves = movePool;
        int moveCount = 0;
        MoveGen::generateLegalMoves(board, moves, moveCount);

        // intilize and set all values to time search
        searchStartTime = chrono::steady_clock::now();
        searchAllocatedMs = allocatedMS;
        stopSearch = false;
        nodesSearched = 0;

        // iterative deepening
        int bestMove = 0;
        int maxDepthReached = 1;
        for (int depth = 1; depth <= MAX_DEPTH; depth++) {
            int candidateMove;
            if (depth == 1) candidateMove = searchRoot(board, depth, moves, moveCount, nodesSearched); 
            else candidateMove = searchRoot(board, depth, moves, moveCount, nodesSearched, bestMove); 
            
            // if time finished, discard previous depth results
            // because not all nodes explored
            if (stopSearch) break;

            maxDepthReached = depth;
            bestMove = candidateMove;
        }
        
        // pass extra info for engine stats
        cout << "info depth " << maxDepthReached << " score cp " << Evaluation::evaluate(board) << " nodes " << nodesSearched << endl;
        return moves[bestMove];
    }
}