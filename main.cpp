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

int main() {
    UCI::loop();
    
    return 0;
}
