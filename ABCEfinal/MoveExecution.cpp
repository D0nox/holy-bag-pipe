#include "MyLibraries.h"

// does not keep previous en passant and previous stable pos bools

void doMove(chessboard& BOARD, MOVE& move, bool White, int& capturedPiece, bool& enPassant, int& tempCastling) {
    capturedPiece = -1;
    int prom_to = 0;
    if (move.to > 63) {
        if (move.to < 72)prom_to = q;
        else if (move.to < 80)prom_to = n;
        else if (move.to < 88)prom_to = b;
        else prom_to = r;
        if (White) {
            move.to = move.to % 8 + 56;
        }
        else {
            move.to = move.to % 8;
        }
    }

    if (get_bit(get_occupancy(getColour(BOARD, !White)), move.to) != 0) {
        hash_delete_piece(move.to, !White, BOARD);
        delete_opponents_piece(BOARD, move.to, White, capturedPiece);
    }
    if (get_bit(getPawn(BOARD, White), move.from) != 0) {
        if (BOARD.en_passant == move.to && move.to != 0) {
            enPassant = true;
            BOARD.keep_en_passant = false;
            BOARD.en_passant = 0;
            if (White) {
                hash_delete_piece(move.to - 8, !White, BOARD);
                pop_bit(getPawn(BOARD, !White), move.to - 8);
            }
            else {
                hash_delete_piece(move.to + 8, !White, BOARD);
                pop_bit(getPawn(BOARD, !White), move.to + 8);
            }
            hash_move_piece(move, White, BOARD);
            move_piece(getColour(BOARD, White), move);
        }
        else if ((White && move.from > a6) || (!White && move.from < h3)) {
            hash_delete_piece(move.from, White, BOARD);
            hash_summon_piece(move.from, White, BOARD, prom_to);
            pop_bit(getPawn(BOARD, White), move.from);
            restore_opponents_piece(BOARD, move.from, !White, prom_to);
            hash_move_piece(move, White, BOARD);
            move_piece(getColour(BOARD, White), move);
        }
        else if ((White && move.from < h3 && move.to > a3) || (!White && move.from > a6 && move.to < h6)) {
            BOARD.keep_en_passant = true;
            if (White)
                BOARD.en_passant = move.from + 8;
            else
                BOARD.en_passant = move.from - 8;
            hash_move_piece(move, White, BOARD);
            move_piece(getColour(BOARD, White), move);
        }
        else {
            hash_move_piece(move, White, BOARD);
            move_piece(getColour(BOARD, White), move);
        }
    }
    else if (get_bit(getKing(BOARD, White), move.from) != 0) {
        tempCastling = BOARD.castling;
        if (White)
            BOARD.castling &= 3;
        else
            BOARD.castling &= 12;

        if (move.from - move.to == 2 || move.from - move.to == -2) {
            int BlackSide = 0;
            if (!White) BlackSide = 56;
            if (move.to == g1 + BlackSide) {
                hash_delete_piece(h1 + BlackSide, White, BOARD);
                hash_summon_piece(f1 + BlackSide, White, BOARD, r);
                hash_move_piece(move, White, BOARD);
                pop_bit(getRook(BOARD, White), h1 + BlackSide);
                set_bit(getRook(BOARD, White), f1 + BlackSide);
                move_piece(getColour(BOARD, White), move);
            }
            else if (move.to == c1 + BlackSide) {
                hash_delete_piece(a1 + BlackSide, White, BOARD);
                hash_summon_piece(d1 + BlackSide, White, BOARD, r);
                hash_move_piece(move, White, BOARD);
                pop_bit(getRook(BOARD, White), a1 + BlackSide);
                set_bit(getRook(BOARD, White), d1 + BlackSide);
                move_piece(getColour(BOARD, White), move);
            }
        }
        else {
            hash_move_piece(move, White, BOARD);
            move_piece(getColour(BOARD, White), move);
        }
    }
    else if (get_bit(getRook(BOARD, White), move.from) != 0) {
        tempCastling = BOARD.castling;
        if (White) {
            if ((BOARD.castling & 4) != 0 && move.from == h1)
                BOARD.castling &= 11;
            else if ((BOARD.castling & 8) != 0 && move.from == a1)
                BOARD.castling &= 7;
        }
        else {
            if ((BOARD.castling & 1) != 0 && move.from == h8)
                BOARD.castling &= 14;
            else if ((BOARD.castling & 2) != 0 && move.from == a8)
                BOARD.castling &= 13;
        }
        hash_move_piece(move, White, BOARD);
        move_piece(getColour(BOARD, White), move);
    }
    else {
        hash_move_piece(move, White, BOARD);
        move_piece(getColour(BOARD, White), move);
    }

    if (prom_to != 0) {
        move.to = 64 + move.to % 8;
        if (prom_to == n)move.to += 8;
        else if (prom_to == b)move.to += 16;
        else if (prom_to == r)move.to += 24;
    }
}
void doMoveHash(chessboard& BOARD, MOVE& move, bool White, int& capturedPiece, bool& enPassant, int& tempCastling) {
    capturedPiece = -1;
    int prom_to = 0;
    if (move.to > 63) {
        if (move.to < 72)prom_to = q;
        else if (move.to < 80)prom_to = n;
        else if (move.to < 88)prom_to = b;
        else prom_to = r;
        if (White) {
            move.to = move.to % 8 + 56;
        }
        else {
            move.to = move.to % 8;
        }
    }

    if (get_bit(get_occupancy(getColour(BOARD, !White)), move.to) != 0) {
        hash_delete_piece(move.to, !White, BOARD);
    }
    if (get_bit(getPawn(BOARD, White), move.from) != 0) {
        if (BOARD.en_passant == move.to && move.to != 0) {
            if (White) {
                hash_delete_piece(move.to - 8, !White, BOARD);
            }
            else {
                hash_delete_piece(move.to + 8, !White, BOARD);
            }
            hash_move_piece(move, White, BOARD);
        }
        else if ((White && move.from > a6) || (!White && move.from < h3)) {
            hash_delete_piece(move.from, White, BOARD);
            hash_summon_piece(move.from, White, BOARD, prom_to);
            hash_move_piece(move, White, BOARD, prom_to);
        }
        else if ((White && move.from < h3 && move.to > a3) || (!White && move.from > a6 && move.to < h6)) {
            hash_move_piece(move, White, BOARD);
        }
        else {
            hash_move_piece(move, White, BOARD);
        }
    }
    else if (get_bit(getKing(BOARD, White), move.from) != 0) {
        if (move.from - move.to == 2 || move.from - move.to == -2) {
            int BlackSide = 0;
            if (!White) BlackSide = 56;
            if (move.to == g1 + BlackSide) {
                hash_delete_piece(h1 + BlackSide, White, BOARD);
                hash_summon_piece(f1 + BlackSide, White, BOARD, r);
                hash_move_piece(move, White, BOARD);
            }
            else if (move.to == c1 + BlackSide) {
                hash_delete_piece(a1 + BlackSide, White, BOARD);
                hash_summon_piece(d1 + BlackSide, White, BOARD, r);
                hash_move_piece(move, White, BOARD);
            }
        }
        else {
            hash_move_piece(move, White, BOARD);
        }
    }
    else if (get_bit(getRook(BOARD, White), move.from) != 0) {
        hash_move_piece(move, White, BOARD);
    }
    else {
        hash_move_piece(move, White, BOARD);
    }

    if (prom_to != 0) {
        move.to = 64 + move.to % 8;
        if (prom_to == n)move.to += 8;
        else if (prom_to == b)move.to += 16;
        else if (prom_to == r)move.to += 24;
    }
}
void undoMove(chessboard& BOARD, MOVE& move, bool White, int& capturedPiece, bool& enPassant, int& tempCastling) {
    int prom_to = 0;
    if (move.to > 63) {
        if (move.to < 72)prom_to = q;
        else if (move.to < 80)prom_to = n;
        else if (move.to < 88)prom_to = b;
        else prom_to = r;
        if (White) {
            move.to = move.to % 8 + 56;
        }
        else {
            move.to = move.to % 8;
        }
    }


    MOVE reversed_move;
    reversed_move.to = move.from;
    reversed_move.from = move.to;


    if (get_bit(getPawn(BOARD, White), move.to) != 0) {
        if (enPassant) {
            enPassant = false;
            hash_move_piece(reversed_move, White, BOARD);
            move_piece(getColour(BOARD, White), reversed_move);
            if (White) {
                hash_summon_piece(move.to - 8, !White, BOARD, p);
                set_bit(getPawn(BOARD, !White), move.to - 8);
            }
            else
            {
                hash_summon_piece(move.to + 8, !White, BOARD, p);
                set_bit(getPawn(BOARD, !White), move.to + 8);
            }
            BOARD.en_passant = move.to;
        }
        else {
            //BOARD.en_passant = 0;
            //BOARD.keep_en_passant = false;
            hash_move_piece(reversed_move, White, BOARD);
            move_piece(getColour(BOARD, White), reversed_move);
        }
    }
    else if (get_bit(getKing(BOARD, White), move.to) != 0) {
        if (move.from - move.to == 2 || move.from - move.to == -2) {
            int BlackSide = 0;
            if (!White) BlackSide = 56;
            if (move.to == g1 + BlackSide) {
                hash_move_piece(reversed_move, White, BOARD);
                hash_delete_piece(f1 + BlackSide, White, BOARD);
                hash_summon_piece(h1 + BlackSide, White, BOARD, r);
                move_piece(getColour(BOARD, White), reversed_move);
                set_bit(getRook(BOARD, White), h1 + BlackSide);
                pop_bit(getRook(BOARD, White), f1 + BlackSide);
            }
            else if (move.to == c1 + BlackSide) {
                hash_move_piece(reversed_move, White, BOARD);
                move_piece(getColour(BOARD, White), reversed_move);
                hash_summon_piece(a1 + BlackSide, White, BOARD, r);
                hash_delete_piece(d1 + BlackSide, White, BOARD);
                set_bit(getRook(BOARD, White), a1 + BlackSide);
                pop_bit(getRook(BOARD, White), d1 + BlackSide);
            }
        }
        else {
            hash_move_piece(reversed_move, White, BOARD);
            move_piece(getColour(BOARD, White), reversed_move);
        }
        BOARD.castling = tempCastling;
    }
    else if (prom_to != 0) {
        hash_move_piece(reversed_move, White, BOARD);
        move_piece(getColour(BOARD, White), reversed_move);
        hash_delete_piece(move.from, White, BOARD);
        hash_summon_piece(move.from, White, BOARD, p);
        delete_piece(BOARD, move.from, White);
        set_bit(getPawn(BOARD, White), move.from);
    }
    else if (get_bit(getRook(BOARD, White), move.to) != 0) {
        hash_move_piece(reversed_move, White, BOARD);
        move_piece(getColour(BOARD, White), reversed_move);
        BOARD.castling = tempCastling;
    }
    else {
        hash_move_piece(reversed_move, White, BOARD);
        move_piece(getColour(BOARD, White), reversed_move);
    }


    if (capturedPiece != -1) {
        hash_summon_piece(move.to, !White, BOARD, capturedPiece);
        restore_opponents_piece(BOARD, move.to, White, capturedPiece);
    }

    if (prom_to != 0) {
        move.to = 64 + move.to % 8;
        if (prom_to == n)move.to += 8;
        else if (prom_to == b)move.to += 16;
        else if (prom_to == r)move.to += 24;
    }
}
void undoMoveHash(chessboard& BOARD, MOVE& move, bool White, int& capturedPiece, bool& enPassant, int& tempCastling) {
    int prom_to = 0;
    if (move.to > 63) {
        if (move.to < 72)prom_to = q;
        else if (move.to < 80)prom_to = n;
        else if (move.to < 88)prom_to = b;
        else prom_to = r;
        if (White) {
            move.to = move.to % 8 + 56;
        }
        else {
            move.to = move.to % 8;
        }
    }


    MOVE reversed_move;
    reversed_move.to = move.from;
    reversed_move.from = move.to;


    if (get_bit(getPawn(BOARD, White), move.to) != 0) {
        if (enPassant) {
            hash_move_piece(reversed_move, White, BOARD);
            if (White) {
                hash_summon_piece(move.to - 8, !White, BOARD, p);
            }
            else
            {
                hash_summon_piece(move.to + 8, !White, BOARD, p);
            }
        }
        else {
            hash_move_piece(reversed_move, White, BOARD);
        }
    }
    else if (get_bit(getKing(BOARD, White), move.to) != 0) {
        if (move.from - move.to == 2 || move.from - move.to == -2) {
            int BlackSide = 0;
            if (!White) BlackSide = 56;
            if (move.to == g1 + BlackSide) {
                hash_move_piece(reversed_move, White, BOARD);
                hash_delete_piece(f1 + BlackSide, White, BOARD);
                hash_summon_piece(h1 + BlackSide, White, BOARD, r);
            }
            else if (move.to == c1 + BlackSide) {
                hash_move_piece(reversed_move, White, BOARD);
                hash_summon_piece(a1 + BlackSide, White, BOARD, r);
            }
        }
        else {
            hash_move_piece(reversed_move, White, BOARD);
        }
    }
    else if (prom_to != 0) {
        hash_move_piece(reversed_move, White, BOARD);
        hash_delete_piece(move.from, White, BOARD, prom_to);
        hash_summon_piece(move.from, White, BOARD, p);
    }
    else if (get_bit(getRook(BOARD, White), move.to) != 0) {
        hash_move_piece(reversed_move, White, BOARD);
    }
    else {
        hash_move_piece(reversed_move, White, BOARD);
    }


    if (capturedPiece != -1) {
        hash_summon_piece(move.to, !White, BOARD, capturedPiece);
    }

    if (prom_to != 0) {
        move.to = 64 + move.to % 8;
        if (prom_to == n)move.to += 8;
        else if (prom_to == b)move.to += 16;
        else if (prom_to == r)move.to += 24;
    }
}