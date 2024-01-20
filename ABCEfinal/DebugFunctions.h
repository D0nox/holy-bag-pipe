#pragma once
void testEval(std::string fen, int EvalFunctionType);
void print_bitboard(uint64_t bitboard);
void print_stats(int depth, double time, uint64_t nodes, int eval, bool white, cheese_boards& FIRST_LAYER_BOARDS);
std::string change_eval_to_readable(int eval, int depth);
std::string change_to_readable_notation(MOVE move, bool white);
void single_perft(std::string fen, int depth);
void print_board(chessboard BOARD);
bool notEqualBoardsDebug(chessboard& tempB, chessboard& BOARD, bool white);
void do_many_perft_tests(std::string file_loc);
void test(std::string fen, int depth);