#include "MyLibraries.h"

std::vector<uint64_t> check_for_three_fold_repetition;
bool has_position_occured_two_times(uint64_t positionHash) {
    if (check_for_three_fold_repetition.size() < 3) return false;
    bool occured = false;
    //std::cout << "searching for:/" << position << "/\n";
    for (const uint64_t& posHASH : check_for_three_fold_repetition) {
        //std::cout << "/" << pos << "/\n";
        if (positionHash == posHASH) {
            if (occured)return true;
            else occured = true;
        }
    }
    return false;
}

void addFutureMove(int depth, MOVE move) {
    new_best_moves_mtx.lock();
    futureMoves[depth] = move;
    new_best_moves_mtx.unlock();
}

void sort_positions(cheese_boards& BOARDS, bool white) {
    chessboard temp_board;
    int temp_eval, tempI;
    MOVE temp_move;
    for (int i = 0; i < BOARDS.n; i++) {
        tempI = i;
        temp_eval = BOARDS.eval[i];
        for (int j = i + 1; j < BOARDS.n; j++) {
            if (white) {
                if (BOARDS.eval[j] > temp_eval) {
                    tempI = j;
                    temp_eval = BOARDS.eval[j];
                }
            }
            else {
                if (BOARDS.eval[j] < temp_eval) {
                    tempI = j;
                    temp_eval = BOARDS.eval[j];
                }
            }
        }
        if (tempI != i) {
            temp_board = BOARDS.board[i];
            BOARDS.board[i] = BOARDS.board[tempI];
            BOARDS.board[tempI] = temp_board;
            BOARDS.eval[tempI] = BOARDS.eval[i];
            BOARDS.eval[i] = temp_eval;
            temp_move = BOARDS.moves[i];
            BOARDS.moves[i] = BOARDS.moves[tempI];
            BOARDS.moves[tempI] = temp_move;
        }
    }
}

int quiescenceSearch(chessboard& BOARD, int depth, bool White) {
    if (depth < -6)return EVAL(BOARD, depth);
    else {
        MOVE capAndProm[64];
        int count = 0;

        generateCapturesAndProm(BOARD, White, capAndProm, count);
        if (count != 0) {
            int capPiece, castling, tempENpass, tempENpass2, tempEval, eval = 100000;
            bool enPassa, tempBoolENpass, tempBoolENpass2;

            if (White)eval = -100000;

            tempENpass2 = BOARD.en_passant;
            tempBoolENpass2 = BOARD.keep_en_passant;

            if (BOARD.keep_en_passant)
                BOARD.keep_en_passant = false;
            else
                BOARD.en_passant = 0;

            tempENpass = BOARD.en_passant;
            tempBoolENpass = BOARD.keep_en_passant;
            nodesQesc += count;
            for (int i = 0; i < count; i++) {
                doMove(BOARD, capAndProm[i], White, capPiece, enPassa, castling);
                tempEval = quiescenceSearch(BOARD, depth - 1, !White);
                if (White) {
                    eval = std::max(eval, tempEval);
                }
                else {
                    eval = std::min(eval, tempEval);
                }
                undoMove(BOARD, capAndProm[i], White, capPiece, enPassa, castling);
                BOARD.en_passant = tempENpass;
                BOARD.keep_en_passant = tempBoolENpass;
            }
            BOARD.en_passant = tempENpass2;
            BOARD.keep_en_passant = tempBoolENpass2;
            return eval;
        }
        else return EVAL(BOARD, depth);
    }
}

void penetration_manager(chessboard& BOARD, int depth, bool White, int alpha, int beta, int& eval) {
    if (depth > 0) {
        MOVES moves;

        //chessboard tempB = BOARD;
        //if (folowBestPrewMoves) {
        //    if (depth > 1) {
        //        moves.captures[0] = futureMoves[depth - 1];
        //        //std::cout << change_to_readable_notation(moves.captures[0], White);
        //        moves.capture_c++;
        //    }
        //    else if ( depth == 1) 
        //        folowBestPrewMoves = false;
        //}

        if (depth == 1 && currentlyAnalysingDepth > 3) {
            int Evallol = EVAL(BOARD, depth);

        }

        bool alpha_beta_not_break = true;

        auto start = std::chrono::high_resolution_clock::now();
        auto end = std::chrono::high_resolution_clock::now();

        int enpassant_temp = BOARD.en_passant, stable_pos_temp = BOARD.position_is_stable, bababoy;
        bool enpassant_bool = BOARD.keep_en_passant, tempEnpassant = false, gagagon;

        int tempCapture, tempCastling;


        if (BOARD.keep_en_passant)
            BOARD.keep_en_passant = false;
        else
            BOARD.en_passant = 0;

        bababoy = BOARD.en_passant;
        gagagon = BOARD.keep_en_passant;

        generateAllMoves(BOARD, White, moves);
        //if (depth == 1) { nodes += MOVE_COUNT(moves);/* nodnos += MOVE_COUNT(moves);*/ }
        /*if (BOARD.hash == hashTemp) {
            std::cout << MOVE_COUNT(moves)<<"\n";
            print_board(BOARD);
            if (moves.capture_c != 0)
                for (int i = 0; i < moves.capture_c && alpha_beta_not_break; i++) {
                    std::cout << change_to_readable_notation(moves.captures[i], White) << ": " << 1 << "\n";
                }
            if (moves.move_c != 0)
                for (int i = 0; i < moves.move_c && alpha_beta_not_break; i++) {
                    std::cout << change_to_readable_notation(moves.moves[i], White) << ": " << 1 << "\n";
                }
        }*/
        if (MOVES_EMPTY(moves)) {
            if (king_in_check(BOARD, White)) {
                if (!White) { eval = 99999 - (DEPTH - depth); }
                else { eval = -99999 + (DEPTH - depth); }
            }
            else {
                if (White) { eval = 0; }
                else if (!White) { eval = 0; }
            }
        }
        else {
            int temp_eval;
            eval = 100000;
            if (White)eval = -100000;

            BOARD.position_is_stable = false;
            if (moves.capture_c != 0)
                for (int i = 0; i < moves.capture_c && alpha_beta_not_break; i++) {
                    doMove(BOARD, moves.captures[i], White, tempCapture, tempEnpassant, tempCastling);

                    penetration_manager(BOARD, depth - 1, !White, alpha, beta, temp_eval);

                    if (White) {
                        if (eval > temp_eval) {
                            addFutureMove(depth - 1, moves.captures[i]);
                        }
                        eval = std::max(eval, temp_eval);
                        alpha = std::max(alpha, eval);
                    }
                    else {
                        if (eval < temp_eval) {
                            addFutureMove(depth - 1, moves.captures[i]);
                        }
                        eval = std::min(eval, temp_eval);
                        beta = std::min(beta, eval);
                    }

                    if (beta <= alpha)
                        alpha_beta_not_break = false;
                    nodes++;

                    undoMove(BOARD, moves.captures[i], White, tempCapture, tempEnpassant, tempCastling);
                    BOARD.en_passant = bababoy;
                    BOARD.keep_en_passant = gagagon;
                }
            BOARD.position_is_stable = true;
            if (moves.move_c != 0)
                for (int i = 0; i < moves.move_c && alpha_beta_not_break; i++) {
                    doMove(BOARD, moves.moves[i], White, tempCapture, tempEnpassant, tempCastling);

                    penetration_manager(BOARD, depth - 1, !White, alpha, beta, temp_eval);


                    if (White) {
                        if (eval > temp_eval) {
                            addFutureMove(depth - 1, moves.moves[i]);
                        }
                        eval = std::max(eval, temp_eval);
                        alpha = std::max(alpha, eval);
                    }
                    else {
                        if (eval < temp_eval) {
                            addFutureMove(depth - 1, moves.moves[i]);
                        }
                        eval = std::min(eval, temp_eval);
                        beta = std::min(beta, eval);
                    }

                    if (beta <= alpha)
                        alpha_beta_not_break = false;
                    nodes++;

                    undoMove(BOARD, moves.moves[i], White, tempCapture, tempEnpassant, tempCastling);
                    BOARD.en_passant = bababoy;
                    BOARD.keep_en_passant = gagagon;
                }
        }
        BOARD.position_is_stable = stable_pos_temp;
        BOARD.keep_en_passant = enpassant_bool;
        BOARD.en_passant = enpassant_temp;
    }
    else {
        if (!BOARD.position_is_stable)
            eval = quiescenceSearch(BOARD, depth, White);

        else eval = EVAL(BOARD, depth);
    }
}

int currentlyAnalysingDepth;

void iterativeDepthAnalysis(chessboard BOARD, bool White, int depth, std::string& move_out, int initial_time) {

    double allowed_time = initial_time / 3000.0;
    auto start_of_all_analysis = std::chrono::high_resolution_clock::now();
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    double time;
    std::cout << line;
    uint64_t total_nodes = 0;
    int eval = 0;
    int cutOffDepth = -1;
    int capturedPiece = -1, tempCastling;
    bool enPassant;
    int alpha = -100000, beta = 100000, Eval = -100000;
    MOVES generated_moves;

    std::string colour;

    generateAllMoves(BOARD, White, generated_moves);

    for (int i = 0; i < MOVE_COUNT(generated_moves); i++) {
        FIRST_LAYER_BOARDS.board[i] = BOARD;
    }

    if (!MOVES_EMPTY(generated_moves)) {
        if (generated_moves.capture_c != 0)
            for (int i = 0; i < generated_moves.capture_c; i++) {
                doMove(FIRST_LAYER_BOARDS.board[FIRST_LAYER_BOARDS.n], generated_moves.captures[i], White, capturedPiece, enPassant, tempCastling);
                FIRST_LAYER_BOARDS.eval[FIRST_LAYER_BOARDS.n] = quiescenceSearch(FIRST_LAYER_BOARDS.board[FIRST_LAYER_BOARDS.n], 0, White);
                FIRST_LAYER_BOARDS.moves[FIRST_LAYER_BOARDS.n++] = generated_moves.captures[i];
            }
        if (generated_moves.move_c != 0)
            for (int i = 0; i < generated_moves.move_c; i++) {
                doMove(FIRST_LAYER_BOARDS.board[FIRST_LAYER_BOARDS.n], generated_moves.moves[i], White, capturedPiece, enPassant, tempCastling);
                FIRST_LAYER_BOARDS.eval[FIRST_LAYER_BOARDS.n] = EVAL(FIRST_LAYER_BOARDS.board[FIRST_LAYER_BOARDS.n], 1);
                FIRST_LAYER_BOARDS.moves[FIRST_LAYER_BOARDS.n++] = generated_moves.moves[i];
            }
    }

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);


    std::cout << "\nEvaluating the position:\n";
    std::cout << "depth      nodes       time        speed      |    eval    | moves\n";

    sort_positions(FIRST_LAYER_BOARDS, White);
    futureMoves[0] = FIRST_LAYER_BOARDS.moves[0];
    print_stats(1, (duration.count() * 1.0), FIRST_LAYER_BOARDS.n, FIRST_LAYER_BOARDS.eval[0], White, FIRST_LAYER_BOARDS);

    //dont analyse if only one move available
    if (FIRST_LAYER_BOARDS.n == 1) {
        std::cout << "\n\nBest move: ";
        move_out = change_to_readable_notation(FIRST_LAYER_BOARDS.moves[0], White);
        if (White)
            colour = (FIRST_LAYER_BOARDS.eval[0] < 0 ? "\033[1;31m" : "\033[1;32m");
        else
            colour = (FIRST_LAYER_BOARDS.eval[0] > 0 ? "\033[1;31m" : "\033[1;32m");

        std::cout << move_out << "\nEval: " << colour << change_eval_to_readable(FIRST_LAYER_BOARDS.eval[0], cutOffDepth) << "\033[0m\n\n" << line;
        std::cout << " Total nodes generated: " << 1 << "\n" << line << "\n\n\n\n";

        FIRST_LAYER_BOARDS.n = 0;
    }
    else if (FIRST_LAYER_BOARDS.n != 0) {
        MAX_DEPTH = 2;
        for (int depth = STARTING_DEPTH; depth <= MAX_DEPTH; depth++) {
            currentlyAnalysingDepth = depth;
            start = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(start_of_all_analysis - start);

            allowed_time -= duration.count() / 1000.0;
            if (allowed_time < 0)break;

            alpha = -100000, beta = 100000, Eval = -100000;

            HOW_MANY_THREADS_AT_THE_SAME_TIME = 1;

            if (White)Eval = 100000;

            //// folowing best prev path
            //if (depth > 2) {
            //    folowBestPrewMoves = true;
            //    penetration_manager(FIRST_LAYER_BOARDS.board[0], depth - 1, !White, alpha, beta, FIRST_LAYER_BOARDS.eval[0]);
            //}

            for (int i = 0; i < FIRST_LAYER_BOARDS.n;) {
                for (int j = 0; (j < HOW_MANY_THREADS_AT_THE_SAME_TIME && j + i < FIRST_LAYER_BOARDS.n); j++) {
                    FIRST_LAYER_BOARDS.eval[i + j] = Eval;
                    threads.push_back(std::thread(penetration_manager, std::ref(FIRST_LAYER_BOARDS.board[i + j]), depth - 1, !White, std::ref(alpha), std::ref(beta), std::ref(FIRST_LAYER_BOARDS.eval[i + j])));//fuck you future me nigger (I commited reverse michael jackson)
                    nodes++;
                }

                for (auto& thread : threads)
                    if (thread.joinable())thread.join();

                for (int j = 0; (j < HOW_MANY_THREADS_AT_THE_SAME_TIME && j + i < FIRST_LAYER_BOARDS.n); j++) {
                    if (White) {
                        alpha = std::max(alpha, FIRST_LAYER_BOARDS.eval[i + j]);
                    }
                    else {
                        beta = std::min(beta, FIRST_LAYER_BOARDS.eval[i + j]);
                    }

                    if (beta <= alpha)break;
                }
                if (beta <= alpha)break;

                i += HOW_MANY_THREADS_AT_THE_SAME_TIME;
                if (HOW_MANY_THREADS_AT_THE_SAME_TIME < 6)
                    HOW_MANY_THREADS_AT_THE_SAME_TIME += 2;
            }
            end = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            time = duration.count() * 1.0;

            sort_positions(FIRST_LAYER_BOARDS, White);

            futureMoves[depth - 1] = FIRST_LAYER_BOARDS.moves[0];

            print_stats(depth, time, nodes, FIRST_LAYER_BOARDS.eval[0], White, FIRST_LAYER_BOARDS);

            //if(depth >= 7 && nodes < 10000000)MAX_DEPTH++;

            total_nodes += nodes;
            REALtotal_nodes += nodes;
            REALtotal_time += time / 1000.0;
            nodes = 0;
            nodesQesc = 0;

            if (FIRST_LAYER_BOARDS.eval[0] <= -99985 || FIRST_LAYER_BOARDS.eval[0] >= 99985) { break; cutOffDepth = depth; }
        }

        check_for_three_fold_repetition.push_back(FIRST_LAYER_BOARDS.board[0].hash);

        std::cout << "\n\nBest move: ";
        if (
            (!has_position_occured_two_times(FIRST_LAYER_BOARDS.board[0].hash) ||
                FIRST_LAYER_BOARDS.eval[0] < 0 && White) ||
            (FIRST_LAYER_BOARDS.eval[0] > 0 && !White) ||
            FIRST_LAYER_BOARDS.n == 1
            ) {
            move_out = change_to_readable_notation(FIRST_LAYER_BOARDS.moves[0], White);
            eval = FIRST_LAYER_BOARDS.eval[0];
        }
        else {
            move_out = change_to_readable_notation(FIRST_LAYER_BOARDS.moves[1], White);
            eval = FIRST_LAYER_BOARDS.eval[1];
            check_for_three_fold_repetition.push_back(FIRST_LAYER_BOARDS.board[1].hash);
        }
        if (eval < -100 || eval > 100) {
            if (White)
                colour = (eval < 0 ? "\033[1;31m" : "\033[1;32m");
            else
                colour = (eval > 0 ? "\033[1;31m" : "\033[1;32m");
        }

        std::cout << move_out << "\nEval: " << colour << change_eval_to_readable(eval, cutOffDepth) << "\033[0m\n\n" << line;
        std::cout << " Total nodes generated: " << total_nodes << "\n" << line << "\n\n\n\n";
        FIRST_LAYER_BOARDS.n = 0;
    }
    else {
        std::cout << (White ? "White" : "Black") << " has no moves in this position\nDo you want to continue?";
        std::string a;
        std::cin >> a;
    }
}
