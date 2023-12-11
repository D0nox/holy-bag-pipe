#pragma once
bool hash_eval_exists(uint64_t& hash);
void generate_hashes_for_board_hashing();
void hash_move_piece(MOVE& move, bool white, chessboard& board);
void hash_move_piece(MOVE& move, bool white, chessboard& board, int pieceValue);
void generate_hash_for_pos(chessboard& board, bool white);
int get_piece_value(bitboards& board, int square);
void hash_delete_piece(int square, bool white, chessboard& board);
void hash_delete_piece(int square, bool white, chessboard& board, int pieceValue);
void hash_summon_piece(int square, bool white, chessboard& board, int piece);
