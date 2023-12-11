#pragma once
#include "ClassesAndStructs.h"
chessboard initialize_chessboard(std::string FenCode, bool& White, int& time);
bool are_chessboards_equal(chessboard& a, chessboard& b);



void move_piece(bitboards& a, MOVE& move);

#define get_bit(bitboard, square) (bitboard & (1ULL << (square)))
#define set_bit(bitboard, square) (bitboard |= ((1ULL) << (square)))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? (bitboard ^= (1ULL << (square))) : 0)

uint64_t get_occupancy(chessboard& a);
uint64_t get_occupancy(bitboards& a);

uint64_t get_queen_attacks(int square, uint64_t& occupancy);

uint64_t get_pawn_moves_and_attacks(uint64_t& occupancy, int square, bool is_white, uint64_t& opponent_occupancy);
uint64_t getPawnMoves(uint64_t& occupancy, int square, bool is_white);
uint64_t get_pawn_attacks(int square, bool white);
uint64_t get_all_pawn_attacks(uint64_t& pawns, bool is_white);
uint64_t get_all_pawn_moves(uint64_t& pawns, uint64_t& occupancy, bool white);
uint64_t get_knight_attacks(int square);
uint64_t get_king_attacks(int square);
uint64_t get_occupancy(bitboards& a);
uint64_t get_occupancy(chessboard& a);
uint64_t get_bishop_attacks(int square, uint64_t occupancy);
uint64_t get_rook_attacks(int square, uint64_t occupancy);

unsigned int find_first_set_bit(uint64_t& x);
int Count_bits(uint64_t bitboard);

void init_sliders_attacks(int is_bishop);

bool has_position_occured_two_times(uint64_t& positionHash);

uint64_t& getPawn(chessboard& BOARD, bool white);
uint64_t& getBishop(chessboard& BOARD, bool white);
uint64_t& getKnight(chessboard& BOARD, bool white);
uint64_t& getRook(chessboard& BOARD, bool white);
uint64_t& getKing(chessboard& BOARD, bool white);
uint64_t& getQueen(chessboard& BOARD, bool white);
bitboards& getColour(chessboard& BOARD, bool white);

void restore_opponents_piece(chessboard& BOARD, int square, bool white, int what_to_restore);
void delete_opponents_piece(chessboard& BOARD, int square, bool white, int& what_to_restore);
void delete_piece(chessboard& BOARD, int square, bool white);

bool king_in_check(chessboard& BOARD, bool white);

void generateCapturesAndProm(chessboard& BOARD, bool White, MOVE captures[], int& capture_c);
void generateMoves(chessboard& BOARD, bool White, MOVES& moves);
void generateAllMoves(chessboard& BOARD, bool White, MOVES& moves);