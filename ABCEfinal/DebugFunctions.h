#pragma once
void print_bitboard(uint64_t bitboard);
void print_stats(int depth, double time, uint64_t nodes, int eval, bool white, cheese_boards& FIRST_LAYER_BOARDS);
std::string change_eval_to_readable(int eval, int depth);
std::string change_to_readable_notation(MOVE move, bool white);