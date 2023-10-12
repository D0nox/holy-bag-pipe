#include <iostream>
#include <string>
#include <vector>
#include <string.h>
#include <unordered_map>
#include <mutex>

#include "ClassesAndStructs.h"
#include "GlobalParameters.h"

int MAX_DEPTH = 6;
const int STARTING_DEPTH = 2;
int HOW_MANY_THREADS_AT_THE_SAME_TIME;
int QUESCENCE_SEARCH_DEPTH = 10;



const std::string line = "---------------------------------------------------------------------------------------------------------------------\n";

int moveCount = 0, My_analysed_moves = 0;



long long int nodes = 0, nodesQesc = 0;
long long int REALtotal_nodes = 0;
double REALtotal_time = 0;

int starting_depth;
MOVE now_analysing_perft;
int moves_analysed_perft = 0;

std::unordered_map<uint64_t, int> HASHES_AND_EVALS;
uint64_t BOARD_HASHES[2][6][64];

// Best move sequence
MOVE futureMoves[20];
bool folowBestPrewMoves = false;

std::mutex new_best_moves_mtx;
