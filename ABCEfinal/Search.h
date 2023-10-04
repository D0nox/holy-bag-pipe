#pragma once
void addFutureMove(int depth, MOVE move);
int quiescenceSearch(chessboard& BOARD, int depth, bool White);
void penetration_manager(chessboard& BOARD, int depth, bool White, int alpha, int beta, int& eval);
void iterativeDepthAnalysis(chessboard BOARD, bool White, int depth, std::string& move_out, int initial_time);
