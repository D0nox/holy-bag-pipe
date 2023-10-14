#include "MyLibraries.h"

uint64_t CASTLINGHASH[16];
uint64_t PIECEHASH[6][64];
uint64_t ENPASSANTHASH[8];
uint64_t BLACKHASH;

uint64_t generate_random_number_mine_diamonds(int a) {
    //std::random_device rd;
    std::mt19937_64 generator(69 + a);
    //uint64_t max_value = (std::numeric_limits<uint64_t>::max)();
    //std::uniform_int_distribution<uint64_t> distribution(0ULL, max_value);
    std::uniform_int_distribution<uint64_t> distribution;
    return distribution(generator);
}
bool hash_eval_exists(uint64_t& hash) {
    return HASHES_AND_EVALS.count(hash);
}
int get_piece_value(bitboards& board, int square) {
    if (get_bit(board.pawns, square))return p;
    else if (get_bit(board.bishops, square))return b;
    else if (get_bit(board.knights, square))return n;
    else if (get_bit(board.rooks, square))return r;
    else if (get_bit(board.queen, square))return q;
    else if (get_bit(board.king, square))return k;
    else return -1;
}
void generate_hashes_for_board_hashing() {
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 64; j++)
            PIECEHASH[i][j] = generate_random_number_mine_diamonds(i + j);

    BLACKHASH = generate_random_number_mine_diamonds(-1);

    for (int i = 0; i < 8; i++)
        ENPASSANTHASH[i] = generate_random_number_mine_diamonds(-2 - i);

    for (int i = 0; i < 16; i++)
        CASTLINGHASH[i] = generate_random_number_mine_diamonds(-10 - i);
}
void hash_move_piece(MOVE& move, bool white, chessboard& board) {
    board.hash ^= PIECEHASH[get_piece_value(getColour(board, white), move.from)][move.from];
    board.hash ^= PIECEHASH[get_piece_value(getColour(board, white), move.from)][move.to];
}
void hash_move_piece(MOVE& move, bool white, chessboard& board, int pieceValue) {
    board.hash ^= PIECEHASH[pieceValue][move.from];
    board.hash ^= PIECEHASH[pieceValue][move.to];

}
void generate_hash_for_pos(chessboard& board, bool white) {
    if (!white) board.hash = BLACKHASH;

    uint64_t temp_occ = get_occupancy(board.white);
    int square;
    while (temp_occ != 0) {
        square = find_first_set_bit(temp_occ);
        pop_bit(temp_occ, square);
        board.hash ^= PIECEHASH[get_piece_value(getColour(board, true), square)][square];
    }
    temp_occ = get_occupancy(board.black);
    while (temp_occ != 0) {
        square = find_first_set_bit(temp_occ);
        pop_bit(temp_occ, square);
        board.hash ^= PIECEHASH[get_piece_value(getColour(board, false), square)][square];
    }
}
void hash_delete_piece(int square, bool white, chessboard& board) {
    if (white)
        board.hash ^= BOARD_HASHES[1][get_piece_value(board.white, square)][square];
    else
        board.hash ^= BOARD_HASHES[0][get_piece_value(board.black, square)][square];
}
void hash_delete_piece(int square, bool white, chessboard& board, int pieceValue) {
    if (white)
        board.hash ^= BOARD_HASHES[1][pieceValue][square];
    else
        board.hash ^= BOARD_HASHES[0][pieceValue][square];
}
void hash_summon_piece(int square, bool white, chessboard& board, int piece) {
    if (piece == rA || piece == rH) piece = r;
    if (white)
        board.hash ^= BOARD_HASHES[1][piece][square];
    else
        board.hash ^= BOARD_HASHES[0][piece][square];
}