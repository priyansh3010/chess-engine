#pragma once
#include <cstdint>
#include <intrin.h>

typedef uint64_t U64;

enum Piece { KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN, NONE };
enum Color { WHITE, BLACK, ALL };

enum Square {
    // comments represnet bitboard squares
    a1, b1, c1, d1, e1, f1, g1, h1, // h8 g8 f8 e8 d8 c8 b8 a8
    a2, b2, c2, d2, e2, f2, g2, h2, // h7 g7 f7 e7 d7 c7 b7 a7
    a3, b3, c3, d3, e3, f3, g3, h3, // h6 g6 f6 e6 d6 c6 b6 a6
    a4, b4, c4, d4, e4, f4, g4, h4, // h5 g5 f5 e5 d5 c5 b5 a5
    a5, b5, c5, d5, e5, f5, g5, h5, // h4 g4 f4 e4 d4 c4 b4 a4
    a6, b6, c6, d6, e6, f6, g6, h6, // h3 g3 f3 e3 d3 c3 b3 a3
    a7, b7, c7, d7, e7, f7, g7, h7, // h2 g2 f2 e2 d2 c2 b2 a2
    a8, b8, c8, d8, e8, f8, g8, h8  // h1 g1 f1 e1 d1 c1 b1 a1
};

const int NUM_SQUARES = 64;
const int NUM_UNIQUE_PIECES = 6;

inline int getLSB(U64 bitboard) {
    unsigned long index;
    _BitScanForward64(&index, bitboard);
    return (int)index;
}
