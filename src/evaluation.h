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

static const int PST[6][64];
namespace Evaluation {

    int evaluate(Board& board);
}