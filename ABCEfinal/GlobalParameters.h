#pragma once
extern int MAX_DEPTH;
extern const int STARTING_DEPTH;
extern int HOW_MANY_THREADS_AT_THE_SAME_TIME;
extern int QUESCENCE_SEARCH_DEPTH;



extern const std::string line;

extern int moveCount, My_analysed_moves;

extern int currentlyAnalysingDepth;

extern long long int nodes, nodesQesc;
extern long long int REALtotal_nodes;
extern double REALtotal_time;

extern int starting_depth;
extern MOVE now_analysing_perft;
extern int moves_analysed_perft;

extern std::unordered_map<uint64_t, int> HASHES_AND_EVALS;

extern uint64_t BOARD_HASHES[2][6][64];
extern uint64_t CASTLINGHASH[16];
extern uint64_t PIECEHASH[2][6][64];
extern uint64_t BLACKHASH;
extern uint64_t ENPASSANTHASH[8];


extern int R;

const int DEPTH = 7;

const bool printColours = false;



const int futilityMargin[2] = {3000, 5000};


// Best move sequence
extern MOVE futureMoves[20];
extern bool folowBestPrewMoves;

extern std::mutex new_best_moves_mtx;
extern std::mutex nodeMutex;
extern std::mutex MUTEX;
extern cheese_boards FIRST_LAYER_BOARDS;
extern std::vector<std::thread> threads;