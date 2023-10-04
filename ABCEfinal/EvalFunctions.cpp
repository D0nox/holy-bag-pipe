#include "MyLibraries.h"

int get_attacked_pieces_value(chessboard& BOARD, uint64_t attacked, bool white_attacking) {
    int temp_nr, return_value = 0;
    if (white_attacking) {
        while (attacked != 0) {
            temp_nr = find_first_set_bit(attacked);
            if (get_bit(BOARD.black.bishops, temp_nr) != 0) return_value += 30;
            else if (get_bit(BOARD.black.pawns, temp_nr) != 0) return_value += 10;
            else if (get_bit(BOARD.black.knights, temp_nr) != 0) return_value += 30;
            else if (get_bit(BOARD.black.queen, temp_nr) != 0) return_value += 90;
            else if (get_bit(BOARD.black.rooks, temp_nr) != 0) return_value += 45;
            pop_bit(attacked, temp_nr);
        }
    }
    else {
        while (attacked != 0) {
            temp_nr = find_first_set_bit(attacked);
            if (get_bit(BOARD.white.bishops, temp_nr) != 0) return_value += 30;
            else if (get_bit(BOARD.white.pawns, temp_nr) != 0) return_value += 10;
            else if (get_bit(BOARD.white.knights, temp_nr) != 0) return_value += 30;
            else if (get_bit(BOARD.white.queen, temp_nr) != 0) return_value += 90;
            else if (get_bit(BOARD.white.rooks, temp_nr) != 0) return_value += 45;
            pop_bit(attacked, temp_nr);
        }
    }
    return return_value;
}
int get_xray_attacks(chessboard& BOARD, uint64_t attack_rays, bool white, char piece, int square) {
    uint64_t all_piece_occupancy = get_occupancy(BOARD), attacked_opponent_pieces;
    int power = 1, returnvalue = 0;
    if (white) {
        while ((attack_rays & all_piece_occupancy) != 0) {
            if (piece == 'q') {
                all_piece_occupancy &= ~get_queen_attacks(square, all_piece_occupancy);
            }
            else if (piece == 'b') {
                all_piece_occupancy &= ~get_bishop_attacks(square, all_piece_occupancy);
            }
            else if (piece == 'r') {
                all_piece_occupancy &= ~get_rook_attacks(square, all_piece_occupancy);
            }
            attacked_opponent_pieces = get_occupancy(BOARD.black) & get_queen_attacks(square, all_piece_occupancy);
            returnvalue += get_attacked_pieces_value(BOARD, attacked_opponent_pieces, true) * pow(0.5, power);
            if ((attacked_opponent_pieces & BOARD.black.king) != 0) returnvalue += 18 * pow(0.5, power);
            power++;
        }
    }
    else {
        while ((attack_rays & all_piece_occupancy) != 0) {
            if (piece == 'q') {
                all_piece_occupancy &= ~get_queen_attacks(square, all_piece_occupancy);
            }
            else if (piece == 'b') {
                all_piece_occupancy &= ~get_bishop_attacks(square, all_piece_occupancy);
            }
            else if (piece == 'r') {
                all_piece_occupancy &= ~get_rook_attacks(square, all_piece_occupancy);
            }
            attacked_opponent_pieces = get_occupancy(BOARD.white) & get_queen_attacks(square, all_piece_occupancy);
            returnvalue += get_attacked_pieces_value(BOARD, attacked_opponent_pieces, false) * pow(0.5, power);
            if ((attacked_opponent_pieces & BOARD.white.king) != 0) returnvalue += 18 * pow(0.5, power);
            power++;
        }
    }
    return returnvalue;
}
int quickEVAL(chessboard& BOARD) {
    int eval = 0, returnEval = 0;
    if (BOARD.white.pawns != 0) eval += Count_bits(BOARD.white.pawns) * pawn_value;
    if (BOARD.white.queen != 0) eval += Count_bits(BOARD.white.queen) * queen_value;
    if (BOARD.white.rooks != 0) eval += Count_bits(BOARD.white.rooks) * rook_value;
    if (BOARD.white.knights != 0) eval += Count_bits(BOARD.white.knights) * knight_value;
    if (BOARD.white.bishops != 0) eval += Count_bits(BOARD.white.bishops) * bishop_value;

    returnEval += eval;
    eval = 0;

    if (BOARD.black.pawns != 0) eval += Count_bits(BOARD.black.pawns) * pawn_value;
    if (BOARD.black.queen != 0) eval += Count_bits(BOARD.black.queen) * queen_value;
    if (BOARD.black.rooks != 0) eval += Count_bits(BOARD.black.rooks) * rook_value;
    if (BOARD.black.knights != 0) eval += Count_bits(BOARD.black.knights) * knight_value;
    if (BOARD.black.bishops != 0) eval += Count_bits(BOARD.black.bishops) * bishop_value;
    return returnEval - eval;
}
int EVAL(chessboard& BOARD, int depth) {
    /*if (hash_eval_exists(BOARD.hash))
        return HASHES_AND_EVALS[BOARD.hash];*/
        //return quickEVAL(BOARD);
    uint64_t white_occupancy = get_occupancy(BOARD.white), all_occupancy = get_occupancy(BOARD), black_occupancy = all_occupancy ^ white_occupancy, temp_something,
        control = 0, passed_pawns = 0, doubled_pawns = 0, isolated_pawns = 0, backwards_pawns = 0, semi_backwards_pawns = 0, backwards_fated_pawns = 0, empty_board = 0;
    int eval = 0, finalEval = 0;
    int temp_n, temp_nr, move_count = DEPTH - depth + 1 + moveCount, n_of_pawns = Count_bits(BOARD.white.pawns) + Count_bits(BOARD.black.pawns), temp_control_square_worth = 10, opponent_pawn_attacks = 0;
    double add_more_detailed_eval = 0;
    int allPieces_n = Count_bits(get_occupancy(BOARD)), quick_eval;

    /*if ((alpha != -100000 && ((quick_eval = quickEVAL(BOARD)) > alpha + 1100)) || (beta != 100000 && ((quick_eval = quickEVAL(BOARD)) < beta - 1100)) && !king_in_check(BOARD, 1) && !king_in_check(BOARD, 0))
        return quick_eval;*/

    double rookValue, queenValue, bishopValue, knightValue;
    if (n_of_pawns < 5) {//open position
        rookValue = 1.1;
        queenValue = 1.3;
        bishopValue = 1.2;
        knightValue = 0.85;
    }
    else if (n_of_pawns < 9) {//semi-open position
        rookValue = 1.1;
        queenValue = 1.2;
        bishopValue = 1.15;
        knightValue = 0.90;
    }
    else if (n_of_pawns < 12) {//semi-closed position
        rookValue = 0.9;
        queenValue = 0.95;
        bishopValue = 1.05;
        knightValue = 1.1;
    }
    else {//closed positions
        rookValue = 0.85;
        queenValue = 0.9;
        bishopValue = 1;
        knightValue = 1.1;
    }

    int ERoMoE = 0;
    if (move_count < 16)
        ERoMoE = -1;
    else if (move_count > 15 && allPieces_n > 20)
        ERoMoE = 0;
    else
        ERoMoE = 1;

    uint64_t temp_pieces;
    temp_pieces = white_occupancy;

    if (ERoMoE == -1) {
        double temp_eval_for_oppening0 = 0, temp_eval_for_oppening1 = 0, temp_eval_for_oppening2 = 0;
        while (temp_pieces != 0) {
            temp_nr = find_first_set_bit(temp_pieces);
            temp_eval_for_oppening1 += general_go_forwardW[temp_nr];
            if (get_bit(level_1_square, temp_nr) != 0)temp_eval_for_oppening2 += 2.5;
            else if (get_bit(level_2_square, temp_nr) != 0)temp_eval_for_oppening2 += 5;
            else if (get_bit(level_3_square, temp_nr) != 0)temp_eval_for_oppening2 += 7.5;
            else if (get_bit(level_4_square, temp_nr) != 0)temp_eval_for_oppening2 += 10;
            pop_bit(temp_pieces, temp_nr);
        }
        add_more_detailed_eval += temp_eval_for_oppening1 + temp_eval_for_oppening1 + (temp_eval_for_oppening1 + temp_eval_for_oppening2) / 2;
    }



    if ((temp_n = Count_bits(BOARD.white.pawns)) != 0) {
        temp_pieces = BOARD.white.pawns;
        eval += temp_n * pawn_value;
        if (ERoMoE > -1) {
            eval += (Count_bits(get_all_pawn_attacks(BOARD.white.pawns, true)) + Count_bits(get_all_pawn_moves(BOARD.white.pawns, all_occupancy, true))) * 5;
            control |= get_all_pawn_attacks(BOARD.white.pawns, true);
        }
        while (temp_pieces != 0) {
            temp_nr = find_first_set_bit(temp_pieces);
            if (ERoMoE == -1) {
                eval += EarlyGameWiderCenterControl[temp_nr];
                eval += EarlyGameCenterControlForPawnsW[temp_nr];
            }
            else if (ERoMoE > -1) {

                if ((BOARD.black.pawns & (trim_left >> temp_nr % 8)) == 0) { set_bit(passed_pawns, temp_nr); }

                pop_bit(BOARD.white.pawns, temp_nr);
                if ((BOARD.white.pawns & (trim_left >> temp_nr % 8)) != 0) { set_bit(doubled_pawns, temp_nr); }
                set_bit(BOARD.white.pawns, temp_nr);

                if ((BOARD.white.pawns & get_king_attacks(temp_nr)) == 0) { set_bit(isolated_pawns, temp_nr); }

                if (get_bit(get_all_pawn_attacks(BOARD.black.pawns, false), temp_nr + 8) != 0) {
                    if (get_bit(get_all_pawn_attacks(BOARD.white.pawns, true), temp_nr + 8) != 0)
                        set_bit(semi_backwards_pawns, temp_nr);
                    else {
                        if (Count_bits(BOARD.black.pawns & get_pawn_attacks(temp_nr + 8, true)) == 2)
                            set_bit(backwards_fated_pawns, temp_nr);
                        else {
                            set_bit(backwards_pawns, temp_nr);
                        }
                    }
                }
            }
            pop_bit(temp_pieces, temp_nr);
        }
    }
    if ((temp_n = Count_bits(BOARD.white.knights)) != 0) {
        eval += temp_n * knight_value * knightValue;
        temp_pieces = BOARD.white.knights;
        while (temp_pieces != 0) {
            temp_nr = find_first_set_bit(temp_pieces);
            if (ERoMoE == -1) {
                eval += EarlyGameWiderCenterControl[temp_nr];
                eval += EarlyGamemoveTheKnightInTheCenter[temp_nr];
            }
            else if (ERoMoE > -1) {
                control |= get_knight_attacks(temp_nr);
                temp_something = get_knight_attacks(temp_nr) & ~all_occupancy;
                temp_control_square_worth = 15;
                eval +=
                    Count_bits(temp_something & level_1_square) * temp_control_square_worth
                    + Count_bits(temp_something & level_2_square) * 1.25 * temp_control_square_worth
                    + Count_bits(temp_something & level_3_square) * 1.56 * temp_control_square_worth
                    + Count_bits(temp_something & level_3_square) * 1.95 * temp_control_square_worth;
                eval += Count_bits(temp_something) / 2 * 3;
            }
            pop_bit(temp_pieces, temp_nr);
        }
    }
    if ((temp_n = Count_bits(BOARD.white.bishops)) != 0) {
        eval += temp_n * bishop_value * bishopValue;
        temp_pieces = BOARD.white.bishops;
        while (temp_pieces != 0) {
            temp_nr = find_first_set_bit(temp_pieces);
            if (ERoMoE == -1) {
                eval += EarlyGameWiderCenterControl[temp_nr];
            }
            else if (ERoMoE > -1) {
                control |= get_bishop_attacks(temp_nr, all_occupancy);
                temp_something = get_bishop_attacks(temp_nr, all_occupancy) & ~all_occupancy;
                temp_control_square_worth = 10;
                eval +=
                    Count_bits(temp_something & level_1_square) * temp_control_square_worth
                    + Count_bits(temp_something & level_2_square) * 1.25 * temp_control_square_worth
                    + Count_bits(temp_something & level_3_square) * 1.56 * temp_control_square_worth
                    + Count_bits(temp_something & level_3_square) * 1.95 * temp_control_square_worth;
                eval += Count_bits(temp_something & get_king_attacks(temp_nr)) * 3;
                eval += get_xray_attacks(BOARD, get_bishop_attacks(temp_nr, empty_board), true, 'b', temp_nr);
            }
            pop_bit(temp_pieces, temp_nr);
        }
    }
    if ((temp_n = Count_bits(BOARD.white.rooks)) != 0) {
        eval += temp_n * rook_value * rookValue;
        temp_pieces = BOARD.white.rooks;
        while (temp_pieces != 0) {
            temp_nr = find_first_set_bit(temp_pieces);
            if (ERoMoE == -1) {
                eval -= rook_value * 0.15;
            }
            else if (ERoMoE > -1) {
                control |= get_rook_attacks(temp_nr, all_occupancy);
                temp_something = get_rook_attacks(temp_nr, all_occupancy) & ~all_occupancy;
                temp_control_square_worth = 10;
                eval +=
                    Count_bits(temp_something & level_1_square) * temp_control_square_worth
                    + Count_bits(temp_something & level_2_square) * 1.25 * temp_control_square_worth
                    + Count_bits(temp_something & level_3_square) * 1.56 * temp_control_square_worth
                    + Count_bits(temp_something & level_3_square) * 1.95 * temp_control_square_worth;
                eval += Count_bits(temp_something & get_king_attacks(temp_nr)) * 3;
                eval += get_xray_attacks(BOARD, get_rook_attacks(temp_nr, empty_board), true, 'r', temp_nr);
            }
            pop_bit(temp_pieces, temp_nr);
        }
    }
    if ((temp_n = Count_bits(BOARD.white.queen)) != 0) {
        eval += temp_n * queen_value * queenValue;
        temp_pieces = BOARD.white.queen;
        while (temp_pieces != 0) {
            temp_nr = find_first_set_bit(temp_pieces);
            if (ERoMoE == -1) {
                eval += EarlyGameWiderCenterControl[temp_nr];
                eval -= queen_value * 0.1;
            }
            else if (ERoMoE > -1) {
                control |= get_queen_attacks(temp_nr, all_occupancy);
                temp_something = get_queen_attacks(temp_nr, all_occupancy) & ~all_occupancy;
                temp_control_square_worth = 15;
                eval +=
                    Count_bits(temp_something & level_1_square) * temp_control_square_worth
                    + Count_bits(temp_something & level_2_square) * 1.25 * temp_control_square_worth
                    + Count_bits(temp_something & level_3_square) * 1.56 * temp_control_square_worth
                    + Count_bits(temp_something & level_3_square) * 1.95 * temp_control_square_worth;
                eval += Count_bits(temp_something & get_king_attacks(temp_nr)) * 3;
                eval += get_xray_attacks(BOARD, get_queen_attacks(temp_nr, empty_board), true, 'q', temp_nr);
            }
            pop_bit(temp_pieces, temp_nr);
        }
    }


    eval += king_value;
    temp_pieces = BOARD.white.king;
    temp_nr = find_first_set_bit(temp_pieces);

    if (ERoMoE < 1) {
        eval += EarlyGameHideKingInTheCorner[temp_nr];
    }
    else if (ERoMoE == 0) {
        control |= get_king_attacks(temp_nr);
        eval += Count_bits(get_king_attacks(temp_nr) & ~all_occupancy) * 10;
    }
    else {
        control |= get_king_attacks(temp_nr);
        eval += ENDGAMEkingcentercontrol[temp_nr];
    }

    //eval pawn structure
    if (ERoMoE > -1) {
        int engame_multiplier = 1;
        if (ERoMoE == 1)engame_multiplier = 2;
        if ((temp_pieces = passed_pawns) != 0) {
            while (temp_pieces != 0) {
                temp_nr = find_first_set_bit(temp_pieces);
                eval += 25 * engame_multiplier * bonus_for_further_passers[temp_nr];//63- for black
                if (get_bit(control, temp_nr) != 0)eval += 25 * engame_multiplier * bonus_for_further_passers[temp_nr];
                else if ((passed_pawns & get_king_attacks(temp_nr)) != 0) {
                    eval += 25 * engame_multiplier * bonus_for_further_passers[temp_nr];//63- for black
                    if (Count_bits(passed_pawns & get_king_attacks(temp_nr)) > 1) add_more_detailed_eval += 1.25;//more eval for 3 or more passers
                }
                pop_bit(temp_pieces, temp_nr);
            }
            //calculating the distance between passed pawns
            if (Count_bits(passed_pawns) > 1) {
                int count = 0, slider = 1, bonus = 0;
                bool ehehehehe = false;
                if ((trim_left & passed_pawns) != 0) {
                    ehehehehe = true;
                }
                while (slider < 8) {
                    if (((trim_left >> slider) & passed_pawns) != 0) {
                        if (ehehehehe)bonus += count * 5;
                        ehehehehe = true;
                        count = 0;
                    }
                    else if (ehehehehe) {
                        count++;
                    }
                    slider++;
                }
                eval += bonus;
            }
        }
        if (doubled_pawns != 0) {
            eval += Count_bits(doubled_pawns) * -20 * engame_multiplier;
        }
        if (isolated_pawns != 0) {
            eval += Count_bits(isolated_pawns) * -20 * engame_multiplier;
        }
        if (backwards_pawns != 0) {
            while (backwards_pawns != 0) {
                temp_nr = find_first_set_bit(backwards_pawns);
                if (temp_nr / 8 < 5)//black > 2
                    eval += -25 * pow(0.5, (temp_nr / 8 - 1)) * engame_multiplier;//black 63-
                pop_bit(backwards_pawns, temp_nr);
            }
        }
        if (semi_backwards_pawns != 0) {
            while (semi_backwards_pawns != 0) {
                temp_nr = find_first_set_bit(semi_backwards_pawns);//black 63-
                if (temp_nr / 8 < 3)
                    eval += -15 * pow(0.5, (temp_nr / 8 - 1)) * engame_multiplier;
                pop_bit(semi_backwards_pawns, temp_nr);
            }
        }
        if (backwards_fated_pawns != 0) {
            while (backwards_fated_pawns != 0) {
                temp_nr = find_first_set_bit(backwards_fated_pawns);//black 63-
                if (temp_nr / 8 < 5)
                    eval += -35 * pow(0.5, (temp_nr / 8 - 1)) * engame_multiplier;
                pop_bit(backwards_fated_pawns, temp_nr);
            }
        }
    }
    //control and defense
    if (ERoMoE > -1) {
        eval += get_attacked_pieces_value(BOARD, (control & black_occupancy), true);
        eval += 0.5 * get_attacked_pieces_value(BOARD, (control & white_occupancy), true);
        if ((temp_n = Count_bits(~control & white_occupancy)) != 0) {
            if (temp_n < 3)eval += temp_n * -5;
            else eval += temp_n * -7;
        }
        add_more_detailed_eval += Count_bits(~control & BOARD.white.pawns) * -0.4;//undefended pawns
    }

    eval += add_more_detailed_eval;

    add_more_detailed_eval = 0;
    control = 0;
    passed_pawns = 0;
    doubled_pawns = 0;
    isolated_pawns = 0;
    backwards_pawns = 0;
    semi_backwards_pawns = 0;
    backwards_fated_pawns = 0;


    finalEval += eval;
    eval = 0;

    temp_pieces = black_occupancy;

    if (ERoMoE == -1) {
        double temp_eval_for_oppening0 = 0, temp_eval_for_oppening1 = 0, temp_eval_for_oppening2 = 0;
        while (temp_pieces != 0) {
            temp_nr = find_first_set_bit(temp_pieces);
            temp_eval_for_oppening1 += general_go_forwardW[63 - temp_nr];
            if (get_bit(level_1_square, temp_nr) != 0)temp_eval_for_oppening2 += 2.5;
            else if (get_bit(level_2_square, temp_nr) != 0)temp_eval_for_oppening2 += 5;
            else if (get_bit(level_3_square, temp_nr) != 0)temp_eval_for_oppening2 += 7.5;
            else if (get_bit(level_4_square, temp_nr) != 0)temp_eval_for_oppening2 += 10;
            pop_bit(temp_pieces, temp_nr);
        }
        add_more_detailed_eval += temp_eval_for_oppening1 + temp_eval_for_oppening1 + (temp_eval_for_oppening1 + temp_eval_for_oppening2) / 2;
    }



    if ((temp_n = Count_bits(BOARD.black.pawns)) != 0) {
        temp_pieces = BOARD.black.pawns;
        eval += temp_n * pawn_value;
        if (ERoMoE > -1) {
            eval += (Count_bits(get_all_pawn_attacks(BOARD.black.pawns, false)) + Count_bits(get_all_pawn_moves(BOARD.black.pawns, all_occupancy, false))) * 5;
            control |= get_all_pawn_attacks(BOARD.black.pawns, false);
        }
        while (temp_pieces != 0) {
            temp_nr = find_first_set_bit(temp_pieces);
            if (ERoMoE == -1) {
                eval += EarlyGameWiderCenterControl[temp_nr];
                eval += EarlyGameCenterControlForPawnsB[temp_nr];
            }
            else if (ERoMoE > -1) {

                if ((BOARD.black.pawns & (trim_left >> temp_nr % 8)) == 0) { set_bit(passed_pawns, temp_nr); }

                pop_bit(BOARD.black.pawns, temp_nr);
                if ((BOARD.black.pawns & (trim_left >> temp_nr % 8)) != 0) { set_bit(doubled_pawns, temp_nr); }
                set_bit(BOARD.black.pawns, temp_nr);

                if ((BOARD.black.pawns & get_king_attacks(temp_nr)) == 0) { set_bit(isolated_pawns, temp_nr); }

                if (get_bit(get_all_pawn_attacks(BOARD.white.pawns, true), temp_nr - 8) != 0) {
                    if (get_bit(get_all_pawn_attacks(BOARD.black.pawns, false), temp_nr - 8) != 0)
                        set_bit(semi_backwards_pawns, temp_nr);
                    else {
                        if (Count_bits(BOARD.black.pawns & get_pawn_attacks(temp_nr - 8, false)) == 2)
                            set_bit(backwards_fated_pawns, temp_nr);
                        else {
                            set_bit(backwards_pawns, temp_nr);
                        }
                    }
                }
            }
            pop_bit(temp_pieces, temp_nr);
        }
    }
    if ((temp_n = Count_bits(BOARD.black.knights)) != 0) {
        eval += temp_n * knight_value * knightValue;
        temp_pieces = BOARD.black.knights;
        while (temp_pieces != 0) {
            temp_nr = find_first_set_bit(temp_pieces);
            if (ERoMoE == -1) {
                eval += EarlyGameWiderCenterControl[temp_nr];
                eval += EarlyGamemoveTheKnightInTheCenter[temp_nr];
            }
            else if (ERoMoE > -1) {
                control |= get_knight_attacks(temp_nr);
                temp_something = get_knight_attacks(temp_nr) & ~all_occupancy;
                temp_control_square_worth = 15;
                eval +=
                    Count_bits(temp_something & level_1_square) * temp_control_square_worth
                    + Count_bits(temp_something & level_2_square) * 1.25 * temp_control_square_worth
                    + Count_bits(temp_something & level_3_square) * 1.56 * temp_control_square_worth
                    + Count_bits(temp_something & level_3_square) * 1.95 * temp_control_square_worth;
                eval += Count_bits(temp_something) / 2 * 3;
            }
            pop_bit(temp_pieces, temp_nr);
        }
    }
    if ((temp_n = Count_bits(BOARD.black.bishops)) != 0) {
        eval += temp_n * bishop_value * bishopValue;
        temp_pieces = BOARD.black.bishops;
        while (temp_pieces != 0) {
            temp_nr = find_first_set_bit(temp_pieces);
            if (ERoMoE == -1) {
                eval += EarlyGameWiderCenterControl[temp_nr];
            }
            else if (ERoMoE > -1) {
                control |= get_bishop_attacks(temp_nr, all_occupancy);
                temp_something = get_bishop_attacks(temp_nr, all_occupancy) & ~all_occupancy;
                temp_control_square_worth = 10;
                eval +=
                    Count_bits(temp_something & level_1_square) * temp_control_square_worth
                    + Count_bits(temp_something & level_2_square) * 1.25 * temp_control_square_worth
                    + Count_bits(temp_something & level_3_square) * 1.56 * temp_control_square_worth
                    + Count_bits(temp_something & level_3_square) * 1.95 * temp_control_square_worth;
                eval += Count_bits(temp_something & get_king_attacks(temp_nr)) * 3;
                eval += get_xray_attacks(BOARD, get_bishop_attacks(temp_nr, empty_board), false, 'b', temp_nr);
            }
            pop_bit(temp_pieces, temp_nr);
        }
    }
    if ((temp_n = Count_bits(BOARD.black.rooks)) != 0) {
        eval += temp_n * rook_value * rookValue;
        temp_pieces = BOARD.black.rooks;
        while (temp_pieces != 0) {
            temp_nr = find_first_set_bit(temp_pieces);
            if (ERoMoE == -1) {
                eval -= rook_value * 0.15;
            }
            else if (ERoMoE > -1) {
                control |= get_rook_attacks(temp_nr, all_occupancy);
                temp_something = get_rook_attacks(temp_nr, all_occupancy) & ~all_occupancy;
                temp_control_square_worth = 10;
                eval +=
                    Count_bits(temp_something & level_1_square) * temp_control_square_worth
                    + Count_bits(temp_something & level_2_square) * 1.25 * temp_control_square_worth
                    + Count_bits(temp_something & level_3_square) * 1.56 * temp_control_square_worth
                    + Count_bits(temp_something & level_3_square) * 1.95 * temp_control_square_worth;
                eval += Count_bits(temp_something & get_king_attacks(temp_nr)) * 3;
                eval += get_xray_attacks(BOARD, get_rook_attacks(temp_nr, empty_board), false, 'r', temp_nr);
            }
            pop_bit(temp_pieces, temp_nr);
        }
    }
    if ((temp_n = Count_bits(BOARD.black.queen)) != 0) {
        eval += temp_n * queen_value * queenValue;
        temp_pieces = BOARD.black.queen;
        while (temp_pieces != 0) {
            temp_nr = find_first_set_bit(temp_pieces);
            if (ERoMoE == -1) {
                eval += EarlyGameWiderCenterControl[temp_nr];
                eval -= queen_value * 0.1;
            }
            else if (ERoMoE > -1) {
                control |= get_queen_attacks(temp_nr, all_occupancy);
                temp_something = get_queen_attacks(temp_nr, all_occupancy) & ~all_occupancy;
                temp_control_square_worth = 15;
                eval +=
                    Count_bits(temp_something & level_1_square) * temp_control_square_worth
                    + Count_bits(temp_something & level_2_square) * 1.25 * temp_control_square_worth
                    + Count_bits(temp_something & level_3_square) * 1.56 * temp_control_square_worth
                    + Count_bits(temp_something & level_3_square) * 1.95 * temp_control_square_worth;
                eval += Count_bits(temp_something & get_king_attacks(temp_nr)) * 3;
                eval += get_xray_attacks(BOARD, get_queen_attacks(temp_nr, empty_board), false, 'q', temp_nr);
            }
            pop_bit(temp_pieces, temp_nr);
        }
    }


    eval += king_value;
    temp_pieces = BOARD.black.king;
    temp_nr = find_first_set_bit(temp_pieces);

    if (ERoMoE < 1) {
        eval += EarlyGameHideKingInTheCorner[temp_nr];
    }
    else if (ERoMoE == 0) {
        eval += Count_bits(get_king_attacks(temp_nr) & ~all_occupancy) * 10;
        control |= get_king_attacks(temp_nr);
    }
    else {
        eval += ENDGAMEkingcentercontrol[temp_nr];
        control |= get_king_attacks(temp_nr);
    }


    //eval pawn structure
    if (ERoMoE > -1) {
        int engame_multiplier = 1;
        if (ERoMoE == 1)engame_multiplier = 2;
        if ((temp_pieces = passed_pawns) != 0) {
            while (temp_pieces != 0) {
                temp_nr = find_first_set_bit(temp_pieces);
                eval += 25 * engame_multiplier * bonus_for_further_passers[63 - temp_nr];//63- for black
                if (get_bit(control, temp_nr) != 0)eval += 25 * engame_multiplier * bonus_for_further_passers[63 - temp_nr];
                else if ((passed_pawns & get_king_attacks(temp_nr)) != 0) {
                    eval += 25 * engame_multiplier * bonus_for_further_passers[63 - temp_nr];//63- for black
                    if (Count_bits(passed_pawns & get_king_attacks(temp_nr)) > 1) add_more_detailed_eval += 1.25;//more eval for 3 or more passers
                }
                pop_bit(temp_pieces, temp_nr);
            }
            //calculating the distance between passed pawns
            if (Count_bits(passed_pawns) > 1) {
                int count = 0, slider = 1, bonus = 0;
                bool ehehehehe = false;
                if ((trim_left & passed_pawns) != 0) {
                    ehehehehe = true;
                }
                while (slider < 8) {
                    if (((trim_left >> slider) & passed_pawns) != 0) {
                        if (ehehehehe)bonus += count * 5;
                        ehehehehe = true;
                        count = 0;
                    }
                    else if (ehehehehe) {
                        count++;
                    }
                    slider++;
                }
                eval += bonus;
            }
        }
        if (doubled_pawns != 0) {
            eval += Count_bits(doubled_pawns) * -20 * engame_multiplier;
        }
        if (isolated_pawns != 0) {
            eval += Count_bits(isolated_pawns) * -20 * engame_multiplier;
        }
        if (backwards_pawns != 0) {
            while (backwards_pawns != 0) {
                temp_nr = find_first_set_bit(backwards_pawns);
                if (temp_nr / 8 > 2)//black > 2
                    eval += -25 * pow(0.5, (63 - (temp_nr / 8 - 1))) * engame_multiplier;//black 63-
                pop_bit(backwards_pawns, temp_nr);
            }
        }
        if (semi_backwards_pawns != 0) {
            while (semi_backwards_pawns != 0) {
                temp_nr = find_first_set_bit(semi_backwards_pawns);//black 63-
                if (temp_nr / 8 < 3)
                    eval += -15 * pow(0.5, (63 - (temp_nr / 8 - 1))) * engame_multiplier;
                pop_bit(semi_backwards_pawns, temp_nr);
            }
        }
        if (backwards_fated_pawns != 0) {
            while (backwards_fated_pawns != 0) {
                temp_nr = find_first_set_bit(backwards_fated_pawns);//black 63-
                if (temp_nr / 8 < 5)
                    eval += -35 * pow(0.5, (63 - (temp_nr / 8 - 1))) * engame_multiplier;
                pop_bit(backwards_fated_pawns, temp_nr);
            }
        }
    }
    //control and defense
    if (ERoMoE > -1) {
        eval += get_attacked_pieces_value(BOARD, (control & white_occupancy), false);
        eval += 0.5 * get_attacked_pieces_value(BOARD, (control & black_occupancy), false);
        if ((temp_n = Count_bits(~control & black_occupancy)) != 0) {
            if (temp_n < 3)eval += temp_n * -5;
            else eval += temp_n * -7;
        }
        add_more_detailed_eval += Count_bits(~control & BOARD.black.pawns) * -0.4;
    }



    eval += add_more_detailed_eval;



    finalEval -= eval;

    /*if (depth > 3)
        HASHES_AND_EVALS[BOARD.hash] = finalEval;*/

    return finalEval;
}