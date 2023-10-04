#pragma once
struct MOVE {
    int from, to;
};
struct MOVES {
    MOVE moves[128], captures[128];
    int move_c = 0, capture_c = 0;
};
struct bitboards
{
    uint64_t pawns = 0;
    uint64_t knights = 0;
    uint64_t bishops = 0;
    uint64_t king = 0;
    uint64_t queen = 0;
    uint64_t rooks = 0;
};
struct chessboard
{
    bitboards white;
    bitboards black;
    int castling = 0;
    int en_passant = 0;//where en passant located
    bool keep_en_passant = 0;//keep en passant for 1 move
    bool position_is_stable = 1;
    uint64_t hash = 0;
};
struct cheese_boards {
    chessboard board[128];
    int eval[128];
    int n = 0;
    MOVE moves[128];
};