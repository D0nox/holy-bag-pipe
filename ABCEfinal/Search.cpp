#include "MyLibraries.h"
#include <future>

std::vector<uint64_t> check_for_three_fold_repetition;
bool has_position_occured_two_times(uint64_t& positionHash) {
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
uint64_t promotion_line_w = 0x7760000000000000;
uint64_t promotion_line_b = 0x177400;
bool arePawnsOnPromotionLine(uint64_t& pawns, bool white) {
    if (white && ((pawns & promotion_line_w) != 0))return true;
    else if (!white && ((pawns & promotion_line_b) != 0))return true;
    else return false;
}

int quiescenceSearch(chessboard& BOARD, int depth, bool White, int alpha, int beta) {
    nodeMutex.lock();
    nodesQesc++;
    nodeMutex.unlock();
    if (depth < -10) { 
        return EVAL(BOARD, depth, White);
    }
    else {
        MOVE capAndProm[128];
        int count = 0;
        int val = EVAL(BOARD, depth, White);
        if (val >= beta)return beta;
        if (val > alpha)
            alpha = val;

        int bigDelta = 900;
        if (arePawnsOnPromotionLine(getPawn(BOARD, White), White))bigDelta += 700;

        if (val < alpha - bigDelta)
            return alpha;
        //chessboard tempB = BOARD;
        int capPiece = -1, castling = BOARD.castling, tempENpass, tempENpass2, tempEval;
        bool enPassa = false, tempBoolENpass, tempBoolENpass2;

        tempENpass2 = BOARD.en_passant;
        tempBoolENpass2 = BOARD.keep_en_passant;

        if (BOARD.keep_en_passant)
            BOARD.keep_en_passant = false;
        else
            BOARD.en_passant = 0;

        generateCapturesAndProm(BOARD, White, capAndProm, count);
        /*if (notEqualBoardsDebug(tempB, BOARD, White)) {
            MOVE capAndProm1[128];
            int count1 = 0;
            generateCapturesAndProm(BOARD, White, capAndProm1, count1);

        }*/
        if (count != 0) {
            

            tempENpass = BOARD.en_passant;
            tempBoolENpass = BOARD.keep_en_passant;
            
            //tempB = BOARD;
            for (int i = 0; i < count; i++) {
                doMove(BOARD, capAndProm[i], White, capPiece, enPassa, castling);

                tempEval = -quiescenceSearch(BOARD, depth - 1, !White, -beta, -alpha);

                undoMove(BOARD, capAndProm[i], White, capPiece, enPassa, castling);
                BOARD.en_passant = tempENpass;
                BOARD.keep_en_passant = tempBoolENpass;
                //if (notEqualBoardsDebug(tempB, BOARD, White)) {
                //    /*capPiece = tempOne;
                //    enPassa = nein;
                //    castling = pemt;*/
                //    print_board(BOARD);
                //    doMove(BOARD, capAndProm[i], White, capPiece, enPassa, castling);
                //    print_board(BOARD);
                //    tempEval = -quiescenceSearch(BOARD, depth - 1, !White, -beta, -alpha);
                //    print_board(BOARD);
                //    undoMove(BOARD, capAndProm[i], White, capPiece, enPassa, castling);
                //    print_board(BOARD);
                //    //notEqualBoardsDebug(tempB, BOARD, White);
                //    MUTEX.unlock();
                //}

                if (tempEval >= beta) {
                    BOARD.en_passant = tempENpass2;
                    BOARD.keep_en_passant = tempBoolENpass2;

                    return beta;
                }
                if (tempEval > alpha)
                    alpha = tempEval;
            }
            BOARD.en_passant = tempENpass2;
            BOARD.keep_en_passant = tempBoolENpass2;
            return alpha;
        }
        else return EVAL(BOARD, depth, White);
    }
}

void alphaBeta(int& alpha, int& beta, int& eval, int& temp_eval, bool& White, bool& alpha_beta_not_break) {
    if (White) {
        /*if (eval < temp_eval) {
            addFutureMove(depth - 1, move);
        }*/
        eval = std::max(eval, temp_eval);
        alpha = std::max(alpha, eval);
    }
    else {
        /*if (eval > temp_eval) {
            addFutureMove(depth - 1, move);
        }*/
        eval = std::min(eval, temp_eval);
        beta = std::min(beta, eval);
    }

    if (beta <= alpha)
        alpha_beta_not_break = false;
}

int bestHashMove(MOVES& moves, chessboard& BOARD, bool White) {
    int I = -1;
    bool enPassant = false;
    int castling = BOARD.castling, capt = -1, bestEval = -infinity;
    for (int i = 0; i < moves.Count; i++) {
        doMoveHash(BOARD, moves.moves[i], White, capt, enPassant, castling);

        if (hash_eval_exists(BOARD.hash) && bestEval < HASHES_AND_EVALS[BOARD.hash]) { bestEval = HASHES_AND_EVALS[BOARD.hash]; I = i; }

        undoMoveHash(BOARD, moves.moves[i], White, capt, enPassant, castling);
    }
    return I;
}

void sortMoves(MOVES& moves, chessboard& BOARD, bool White, int depth) {
    if (currentlyAnalysingDepth - depth < currentlyAnalysingDepth - 1 && hash_eval_exists(BOARD.hash)) {
        MOVE tempMove;
        int temp = bestHashMove(moves, BOARD, White);
        if (temp > 0) {
            tempMove = moves.moves[0];
            moves.moves[0] = moves.moves[temp];
            for (int i = 1; i < temp; i++) {
                moves.moves[temp] = tempMove;
                tempMove = moves.moves[i];
                moves.moves[i] = moves.moves[temp];
            }
        }
    }
}

int nullMove(chessboard& BOARD, bool White, int depth, int alpha, int beta) {
    BOARD.canNullMove = false;
    int score =  penetration_manager(BOARD, depth - R - 1, White, alpha, beta);
    BOARD.canNullMove = true;
    return score;
}

int penetration_manager(chessboard& BOARD, int depth, bool White, int alpha, int beta) {
    nodeMutex.lock();
    nodes++;
    nodeMutex.unlock();
    if (depth > 0) {
        bool ownKingInCheck = king_in_check(BOARD, White);

        /*if (depth == 1 && ownKingInCheck)
            depth++;*/

        /*if (
            depth > 2 &&
            BOARD.canNullMove &&
            ownKingInCheck &&
            nullMove(BOARD, White, depth, alpha, beta) >= beta
            )
            return beta;*/

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
        //        //execute reduced search here !
        //}

        /*if (depth == 1 && currentlyAnalysingDepth > 3) {
            int Evallol = EVAL(BOARD, depth);

        }*/

        bool alpha_beta_not_break = true;


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

        
        //sortMoves(moves, BOARD, White, depth);


        //if (depth == 1) nodes += moves.Count;
        if (moves.Count == 0) {
            if (ownKingInCheck) {
                return checkmate + (currentlyAnalysingDepth - depth);
            }
            else
                return 0;
        }
        else {
            int score, posEval;
            if(depth <= 2) posEval = EVAL(BOARD, depth, White);

            if (moves.Count != 0)
                for (int i = 0; i < moves.Count && alpha_beta_not_break; i++) {
                    doMove(BOARD, moves.moves[i], White, tempCapture, tempEnpassant, tempCastling);
                    BOARD.hash ^= BLACKHASH;

                    if (tempCapture != -1) {
                        BOARD.position_is_stable = false;
                    }
                    if (depth <= 2) {
                        if ((!ownKingInCheck || tempCapture == -1 || !king_in_check(BOARD, !White)) && posEval < alpha - futilityMargin[depth - 1])
                            score = posEval;
                        else
                            score = -penetration_manager(BOARD, depth - 1, !White, -beta, -alpha);;
                    }
                    else {
                        score = -penetration_manager(BOARD, depth - 1, !White, -beta, -alpha);
                    }
                    

                    //HASHES_AND_EVALS[BOARD.hash] = eval;

                    
                    BOARD.hash ^= BLACKHASH;
                    undoMove(BOARD, moves.moves[i], White, tempCapture, tempEnpassant, tempCastling);
                    BOARD.en_passant = bababoy;
                    BOARD.keep_en_passant = gagagon;
                    BOARD.position_is_stable = stable_pos_temp;


                    if (score >= beta) {
                        BOARD.keep_en_passant = enpassant_bool;
                        BOARD.en_passant = enpassant_temp;
                        return beta;
                    }
                    if (score > alpha)
                        alpha = score;
                }

            /*if(depth > 2)
                HASHES_AND_EVALS[BOARD.hash] = alpha;*/
            BOARD.keep_en_passant = enpassant_bool;
            BOARD.en_passant = enpassant_temp;
            return alpha;
        }
    }
    else {
        if (!BOARD.position_is_stable)
            return quiescenceSearch(BOARD, depth, White, alpha, beta);

        else return EVAL(BOARD, depth, White);
    }
}

int currentlyAnalysingDepth;

void iterativeDepthAnalysis(chessboard& BOARD, bool White, int depth, std::string& move_out, int initial_time) {
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

    cheese_boards FIRST_LAYER_BOARDS;
    

    std::vector<std::thread> threads;
    std::vector<std::future<int>> futures;


    generateAllMoves(BOARD, White, generated_moves);

    for (int i = 0; i < generated_moves.Count; i++) {
        FIRST_LAYER_BOARDS.board[i] = BOARD;
    }

    if (generated_moves.Count != 0)
        for (int i = 0; i < generated_moves.Count; i++) {
            doMove(FIRST_LAYER_BOARDS.board[FIRST_LAYER_BOARDS.n], generated_moves.moves[i], White, capturedPiece, enPassant, tempCastling);
            if (capturedPiece == -1)
                FIRST_LAYER_BOARDS.eval[FIRST_LAYER_BOARDS.n] = EVAL(FIRST_LAYER_BOARDS.board[FIRST_LAYER_BOARDS.n], 1, White);
            else
                FIRST_LAYER_BOARDS.eval[FIRST_LAYER_BOARDS.n] = -quiescenceSearch(FIRST_LAYER_BOARDS.board[FIRST_LAYER_BOARDS.n], 0, !White, -beta, -alpha);
            FIRST_LAYER_BOARDS.moves[FIRST_LAYER_BOARDS.n++] = generated_moves.moves[i];
        }

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    
    std::cout << "\nEvaluating the position:\n";
    std::cout << "depth      nodes       time        speed      |    eval    | moves\n";

    sort_positions(FIRST_LAYER_BOARDS, White);
    futureMoves[0] = FIRST_LAYER_BOARDS.moves[0];
    print_stats(1, (duration.count() * 1.0), FIRST_LAYER_BOARDS.n, FIRST_LAYER_BOARDS.eval[0], White, FIRST_LAYER_BOARDS);

    nodesQesc = 0;

    R = 3;

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
        MAX_DEPTH = depth;
        for (int depth = STARTING_DEPTH; depth <= MAX_DEPTH; depth++) {
            currentlyAnalysingDepth = depth;

            start = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(start_of_all_analysis - start);

            allowed_time -= duration.count() / 1000.0;
            if (allowed_time < 0)break;

            // Eval = -100000;

            HOW_MANY_THREADS_AT_THE_SAME_TIME = 6;

            //if (White)Eval = 100000;

            //// folowing best prev path
            //if (depth > 2) {
            //    folowBestPrewMoves = true;
            //    penetration_manager(FIRST_LAYER_BOARDS.board[0], depth - 1, !White, alpha, beta, FIRST_LAYER_BOARDS.eval[0]);
            //}
            for (int i = 0; i < FIRST_LAYER_BOARDS.n;) {
                for (int j = i; (j < i + HOW_MANY_THREADS_AT_THE_SAME_TIME && j < FIRST_LAYER_BOARDS.n); j++) {
                    //FIRST_LAYER_BOARDS.eval[j] = Eval;
                    //print_board(FIRST_LAYER_BOARDS.board[j]);
                    //std::cout << std::setw(5) << std::left << change_to_readable_notation(FIRST_LAYER_BOARDS.moves[j], White) << "\n";
                    std::packaged_task<int(chessboard&, int, bool, int, int)> task(penetration_manager);
                    futures.emplace_back(task.get_future());
                    threads.emplace_back(std::thread(std::move(task), std::ref(FIRST_LAYER_BOARDS.board[j]), depth - 1, !White, -beta, -alpha));
                    //threads.push_back(std::thread(penetration_manager, std::ref(FIRST_LAYER_BOARDS.board[i + j]), depth - 1, !White, alpha, beta));//fuck you future me nigger (I commited reverse michael jackson)
                    nodes++;
                }
                for (int j = 0; (j < HOW_MANY_THREADS_AT_THE_SAME_TIME && j + i < FIRST_LAYER_BOARDS.n); j++) {

                    if (threads[j].joinable())threads[j].join();
                }
                //std::cout << "\n";
                for (int j = 0; (j < HOW_MANY_THREADS_AT_THE_SAME_TIME && j + i < FIRST_LAYER_BOARDS.n); j++) {
                    FIRST_LAYER_BOARDS.eval[j + i] = -futures[j].get();
                    //std::cout << "|" << change_eval_to_readable(FIRST_LAYER_BOARDS.eval[j + i], depth) << "|\n\n";
                    //print_board(FIRST_LAYER_BOARDS.board[i + j]);
                    //if (FIRST_LAYER_BOARDS.eval[j + i] > alpha) alpha = FIRST_LAYER_BOARDS.eval[j + i];
                }
                threads.clear();
                futures.clear();

                i += HOW_MANY_THREADS_AT_THE_SAME_TIME;
                /*if (HOW_MANY_THREADS_AT_THE_SAME_TIME < 6)
                    HOW_MANY_THREADS_AT_THE_SAME_TIME += 2;*/
            }
            end = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            time = duration.count() * 1.0;

            sort_positions(FIRST_LAYER_BOARDS, White);

            futureMoves[0] = FIRST_LAYER_BOARDS.moves[0];

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
            (
                !has_position_occured_two_times(FIRST_LAYER_BOARDS.board[0].hash)
                ||
                (
                    (
                        FIRST_LAYER_BOARDS.eval[1] > 0 
                        ||
                        FIRST_LAYER_BOARDS.eval[0] < 0
                    ) 
                    &&
                    White
                )
            )
            ||
            (
                FIRST_LAYER_BOARDS.eval[0] > 0 && !White
            ) 
            ||
            FIRST_LAYER_BOARDS.n == 1
           )
        {
            move_out = change_to_readable_notation(FIRST_LAYER_BOARDS.moves[0], White);
            eval = FIRST_LAYER_BOARDS.eval[0];
        }
        else {
            std::cout << "First move was repetition, took second move\n";
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
