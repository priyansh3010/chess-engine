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
    // Your main logic goes here

    /*
    Board board;
    board.init();
    // board.printBoard();
    
    MoveGen::init();
    
    auto start = chrono::high_resolution_clock::now();
    int nodes = perftDivide(board, 5);
    auto end = chrono::high_resolution_clock::now();
    
    board.printBoard();
    cout << "End of program" << endl;
    
    double seconds = std::chrono::duration<double>(end - start).count();
    std::cout << "NPS: " << (uint64_t)(nodes / seconds) << std::endl;
    */

    UCI::loop();

    return 0;
}
