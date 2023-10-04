#include "MyLibraries.h"

std::string stringRound(double val, int round_to) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(round_to) << val;
    std::string rounded_string = stream.str();
    return rounded_string;
}
void print_board(chessboard BOARD) {
    std::cout << "\n";
    uint64_t pointer = 0x8000000000000000ULL, occupied = get_occupancy(BOARD), black = get_occupancy(BOARD.black), white = get_occupancy(BOARD.white);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if ((occupied & pointer) != 0) {
                if ((white & pointer) != 0) {
                    if ((BOARD.white.bishops & pointer) != 0)std::cout << "B ";
                    else if ((BOARD.white.pawns & pointer) != 0)std::cout << "P ";
                    else if ((BOARD.white.knights & pointer) != 0)std::cout << "N ";
                    else if ((BOARD.white.queen & pointer) != 0)std::cout << "Q ";
                    else if ((BOARD.white.rooks & pointer) != 0)std::cout << "R ";
                    else if ((BOARD.white.king & pointer) != 0)std::cout << "K ";
                }
                else {
                    if ((BOARD.black.bishops & pointer) != 0)std::cout << "b ";
                    else if ((BOARD.black.pawns & pointer) != 0)std::cout << "p ";
                    else if ((BOARD.black.knights & pointer) != 0)std::cout << "n ";
                    else if ((BOARD.black.queen & pointer) != 0)std::cout << "q ";
                    else if ((BOARD.black.rooks & pointer) != 0)std::cout << "r ";
                    else if ((BOARD.black.king & pointer) != 0)std::cout << "k ";
                }
            }
            else if ((7 - i) * 8 + (7 - j) == BOARD.en_passant && BOARD.en_passant != 0)std::cout << "# ";
            else
                std::cout << "- ";
            pointer >>= 1;
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    if ((white & black) != 0) { std::cout << "overlapping squares: "; print_bitboard((white & black)); }
    if ((BOARD.white.bishops & BOARD.white.pawns & BOARD.white.knights & BOARD.white.queen & BOARD.white.rooks & BOARD.white.king) != 0)
    {
        std::cout << "overlapping squares: "; print_bitboard((BOARD.white.bishops & BOARD.white.pawns & BOARD.white.knights & BOARD.white.queen & BOARD.white.rooks & BOARD.white.king));
    }
    if ((BOARD.black.bishops & BOARD.black.pawns & BOARD.black.knights & BOARD.black.queen & BOARD.black.rooks & BOARD.black.king) != 0)
    {
        std::cout << "overlapping squares: "; print_bitboard((BOARD.black.bishops & BOARD.black.pawns & BOARD.black.knights & BOARD.black.queen & BOARD.black.rooks & BOARD.black.king));
    }
}
std::string change_eval_to_readable(int eval, int depth) {
    std::string evalS;

    if (depth % 2 != 0)depth++;
    depth /= 2;

    if (eval >= 99990 || eval <= -99990)evalS = "M" + depth;

    else evalS = stringRound((eval / 1000.0), 2);

    return evalS;
}
std::string change_to_readable_notation(MOVE move, bool white) {
    std::string  prom_to = "";
    if (move.to > 63) {
        if (move.to < 72)prom_to = 'q';
        else if (move.to < 80)prom_to = 'n';
        else if (move.to < 88)prom_to = 'b';
        else prom_to = 'r';
        if (white) {
            move.to = move.to % 8 + 56;
        }
        else {
            move.to = move.to % 8;
        }
    }

    return normal_notation[move.from] + normal_notation[move.to] + prom_to;
}
void print_bitboard(uint64_t bitboard)
{
    printf("\n");

    // loop over board ranks
    for (int rank = 7; rank > -1; rank--)
    {
        // loop over board files
        for (int file = 7; file > -1; file--)
        {
            // init board square
            int square = rank * 8 + file;

            // print ranks
            if (!(file - 7))
                printf("  %d ", rank + 1);

            // print bit indexed by board square
            printf(" %d", get_bit(bitboard, square) ? 1 : 0);
        }

        printf("\n");
    }

    // print files
    printf("\n     a b c d e f g h\n\n");

    // print bitboard as decimal
    printf("     bitboard: %llud\n\n", bitboard);
}
void print_stats(int depth, double time, uint64_t nodes, int eval, bool white, cheese_boards& FIRST_LAYER_BOARDS) {
    std::cout << std::setw(3) << std::right << depth << "    ";

    std::string colour;
    if (white)
        colour = (eval < 0 ? "\033[1;31m" : "\033[1;32m");
    else
        colour = (eval > 0 ? "\033[1;31m" : "\033[1;32m");

    if (nodes < 1000)std::cout << std::setw(6) << std::right << nodes << "  N ";
    else if (nodes < 1000000)std::cout << std::setw(6) << std::right << std::fixed << std::setprecision(2) << nodes / 1000.0 << " kN ";
    else std::cout << std::setw(6) << std::right << std::fixed << std::setprecision(2) << nodes / 1000000.0 << " mN ";

    if (time < 1000)std::cout << std::setw(7) << std::right << std::fixed << std::setprecision(0) << time << " ms   ";
    else std::cout << std::setw(7) << std::right << std::fixed << std::setprecision(1) << time / 1000.0 << "  s   ";

    if (time == 0)std::cout << "   undf" << " kN/s    | ";
    else if (nodes / time < 100)std::cout << std::setw(7) << std::right << std::fixed << std::setprecision(2) << (nodes / time / 1.0) << " kN/s    | ";
    else std::cout << std::setw(7) << std::right << std::fixed << std::setprecision(2) << (nodes / time / 1000.0) << " mN/s    | ";

    //cout << eval;

    std::cout << colour << std::setw(7) << std::right << change_eval_to_readable(eval, depth) << "\033[0m    | ";

    /*for(int i = 0; i < 1 && i < FIRST_LAYER_BOARDS.n; i ++)
        std::cout << change_to_readable_notation(FIRST_LAYER_BOARDS.moves[i], white) << " " << std::setw(6) << std::right << change_eval_to_readable(FIRST_LAYER_BOARDS.eval[i], depth)<< "| ";
    std::cout << "\n";*/

    for (int i = 0; i < 10 && i < depth; i++) {
        std::cout << std::setw(5) << std::left << change_to_readable_notation(futureMoves[depth - i - 1], white) << " ";
    }
    std::cout << "\n"; std::cout << nodesQesc << "\n";
}
uint64_t perft(std::string fen, int depth) {
    int idc;
    bool white;
    chessboard BOARD = initialize_chessboard(fen, white, idc), temp = BOARD;
    penetration_manager(BOARD, depth, white, idc, idc, idc);
    if (!are_chessboards_equal(BOARD, temp)) {
        std::cout << fen << " broke";
        exit(1);
    }
    uint64_t nonodes = nodes;
    nodes = 0, nodesQesc = 0;
    return nonodes;
}
void single_perft(std::string fen, int depth) {
    int idc, total = 0;
    bool white;
    chessboard BOARD = initialize_chessboard(fen, white, idc), tempB = BOARD;

    MOVES generated_moves, haha;
    print_board(BOARD);
    /*generateAllMoves(BOARD, white, generated_moves);
    std::cout << "\nBoard is" << (are_chessboards_equal(tempB, BOARD) ? " " : " not ") << "preserved\n";
    if (!MOVES_EMPTY(generated_moves)) {
        if (generated_moves.capture_c != 0)
            for (int i = 0; i < generated_moves.capture_c; i++) {
                FIRST_LAYER_BOARDS.moves[FIRST_LAYER_BOARDS.n++] = generated_moves.captures[i];
            }
        if (generated_moves.move_c != 0)
            for (int i = 0; i < generated_moves.move_c; i++) {
                FIRST_LAYER_BOARDS.moves[FIRST_LAYER_BOARDS.n++] = generated_moves.moves[i];
            }
    }
    for (int j = 0; j < FIRST_LAYER_BOARDS.n; j++) {
        doMove(BOARD, FIRST_LAYER_BOARDS.moves[j], white, b, a, c);
        penetration_manager(BOARD, 3, white, idc, idc, idc);
        undoMove(BOARD, FIRST_LAYER_BOARDS.moves[j], white, b, a, c);
        std::cout <<nodes<< " " << change_to_readable_notation(FIRST_LAYER_BOARDS.moves[j], white) << "\n";
        total += nodes;
        nodes = 0;
    }
    std::cout << total;*/

    std::cout << "\nposition: " << fen << "\n\n";
    for (int i = 1; i <= depth; i++) {
        std::cout << "\n";
        auto start = std::chrono::high_resolution_clock::now();
        starting_depth = depth;
        //print_board(BOARD);
        penetration_manager(BOARD, i, white, idc, idc, idc);
        //print_board(BOARD);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << i << " D | "
            << nodes << " N | "
            << std::fixed << std::setprecision(3) << (duration.count() / 1000.0) << " s | "
            << std::fixed << std::setprecision(3) << (nodes / (duration.count() / 1000.0)) / 1000000 << " MN/s | \n";//\n|";
        nodes = 0, nodesQesc = 0;
        /*for (int j = 0; j < FIRST_LAYER_BOARDS.n; j++)
            std::cout << change_to_readable_notation(FIRST_LAYER_BOARDS.moves[j], white) << "|";*/
        std::cout << "\nBoard is" << (are_chessboards_equal(tempB, BOARD) ? " " : " not ") << "preserved\n";
        if (!are_chessboards_equal(tempB, BOARD)) {
            std::cout << "Should be:\n";
            print_board(tempB);
            std::cout << "Is:\n";
            print_board(BOARD);

            std::cout << "castling: " << (BOARD.castling == tempB.castling ? "unchanged" : "changed") << "\n";
            std::cout << "hash: " << (BOARD.hash == tempB.hash ? "unchanged" : "changed") << "\n";
            std::cout << "position_is_stable: " << (BOARD.position_is_stable == tempB.position_is_stable ? "unchanged" : "changed") << "\n";
            std::cout << "castling: " << (BOARD.castling == tempB.castling ? "unchanged" : "changed") << "\n";
            std::cout << "en_passant: " << (BOARD.en_passant == tempB.en_passant ? "unchanged" : "changed") << "\n";
            std::cout << "keep_en_passant: " << (BOARD.keep_en_passant == tempB.keep_en_passant ? "unchanged" : "changed") << "\n";
            std::cout << "colour: " << (white ? "white" : "black") << "\n";
        }
    }

}
struct perft_data {
    std::string fen = "";
    std::vector<long long> perft_values;
    std::vector<long long> engine_generated_perft_values;
};
void do_many_perft_tests(std::string file_loc) {
    std::ifstream file(file_loc);
    if (!file) {
        std::cerr << "Failed to open edp perft file";
        exit(1);
    }

    std::string line;
    std::vector<perft_data> perft_data_list;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        perft_data perft_data;

        std::string fen;
        for (int i = 0; i < 5; i++) {
            iss >> fen;
            perft_data.fen += fen + " ";
        }
        iss >> fen;


        char semicolon;
        std::string depth;
        long long perft_value;

        while (iss >> semicolon >> depth >> perft_value) {
            perft_data.perft_values.push_back(perft_value);
        }

        perft_data_list.push_back(perft_data);
    }

    file.close();

    long long int nodess = 0, time = 0, anal_speed = 0, total_nodes = 0;

    int whatever = perft_data_list.size(), whatever2;
    std::cout << "Counting nodes...\n";
    for (int i = 0; i < whatever; i++) {
        std::cout << "\r";
        std::cout.flush();
        std::cout << "[";
        for (int j = 0; j < whatever / 2; j++) {
            if (j < i / 2)std::cout << "#";
            else std::cout << " ";
        }
        std::cout << "] " << std::fixed << std::setprecision(0) << i * 1.0 / whatever * 100 << "% (" << i << "/" << whatever << ") ";/*<<fixed<<setprecision(2)<<(anal_speed / 1000000.0)<<"MN/s "<<fixed << setprecision(2)<<nodess/1000000.0<<"MN"*/;
        total_nodes += nodess / 1000;
        nodess = 0;
        auto start = std::chrono::high_resolution_clock::now();
        whatever2 = perft_data_list[i].perft_values.size();
        for (int j = 0; j < whatever2; j++) {
            perft_data_list[i].engine_generated_perft_values.push_back(perft(perft_data_list[i].fen, j + 1));
            nodess += perft_data_list[i].engine_generated_perft_values[j];
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        //anal_speed = nodess / (duration.count() / 1000.0);
        time += duration.count() / 1000;
        std::cout << "              ";
    }
    std::cout << "\r";
    std::cout.flush();
    std::cout << "[";
    for (int j = 0; j < whatever / 2; j++) {
        std::cout << "#";
    }
    std::cout << "] 100% (" << whatever << "/" << whatever << ")\n";
    int total = 0, correct = 0;
    for (int i = 0; i < whatever; i++) {
        whatever2 = perft_data_list[i].perft_values.size();
        for (int j = 0; j < whatever2; j++) {
            if (perft_data_list[i].perft_values[j] == perft_data_list[i].engine_generated_perft_values[j])correct++;
            total++;
        }
    }
    std::cout << std::fixed << std::setprecision(2) << (correct / total * 100) << "% (" << correct << "/" << total << ") correct\n";
    std::cout << "Duration: " << time << "s\nTotal nodes: " << std::fixed << std::setprecision(2) << total_nodes / 1000.0 << "mN\nAnal speed: " << std::fixed << std::setprecision(2) << (total_nodes / time / 1000.0) << "MN/s\n\n";
    if (correct < total) {
        std::string ans;
        std::cout << "Do you want to see the mistakes?(y/n)";
        std::cin >> ans;
        if (ans == "y") {
            std::cout << "\n";
            for (int i = 0; i < whatever; i++) {
                whatever2 = perft_data_list[i].perft_values.size();
                for (int j = 0; j < whatever2; j++) {
                    if (perft_data_list[i].perft_values[j] != perft_data_list[i].engine_generated_perft_values[j])std::cout << perft_data_list[i].fen << " at depth " << j + 1 << " " << perft_data_list[i].engine_generated_perft_values[j] << " N instead of " << perft_data_list[i].perft_values[j] << " N\n";
                }
            }
        }
    }

}

void test(std::string fen, int depth) {
    bool White;
    int time;
    std::string best_move;
    chessboard BOARD = initialize_chessboard(fen, White, time);
    iterativeDepthAnalysis(BOARD, White, depth, best_move, time);
}

#include <fcntl.h>
#include <io.h>

//sussy teritory
//void printMeme(int memeNumber) {
//    if (memeNumber == 0)std::cout << "卐";
//    else if (memeNumber == 420) {
//        _setmode(_fileno(stdout), 0x00020000);
//        wprintf(L"   ⣞⢽⢪⢣⢣⢣⢫⡺⡵⣝⡮⣗⢷⢽⢽⢽⣮⡷⡽⣜⣜⢮⢺⣜⢷⢽⢝⡽⣝\n  ⠸⡸⠜⠕⠕⠁⢁⢇⢏⢽⢺⣪⡳⡝⣎⣏⢯⢞⡿⣟⣷⣳⢯⡷⣽⢽⢯⣳⣫⠇\n  ⢀⢀⢄⢬⢪⡪⡎⣆⡈⠚⠜⠕⠇⠗⠝⢕⢯⢫⣞⣯⣿⣻⡽⣏⢗⣗⠏\n    ⠪⡪⡪⣪⢪⢺⢸⢢⢓⢆⢤⢀ ⠈⢊⢞⡾⣿⡯⣏⢮⠷⠁\n  ⠈⠊⠆⡃⠕⢕⢇⢇⢇⢇⢇⢏⢎⢎⢆⢄ ⢑⣽⣿⢝⠲⠉\n  ⡿⠂⠠ ⡇⢇⠕⢈⣀ ⠁⠡⠣⡣⡫⣂⣿⠯⢪⠰⠂\n  ⡦⡙⡂⢀⢤⢣⠣⡈⣾⡃⠠⠄ ⡄⢱⣌⣶⢏⢊⠂\n  ⢝⡲⣜⡮⡏⢎⢌⢂⠙⠢⠐⢀⢘⢵⣽⣿⡿⠁⠁\n  ⠨⣺⡺⡕⡕⡱⡑⡆⡕⡅⡕⡜⡼⢽⡻⠏\n  ⣼⣳⣫⣾⣵⣗⡵⡱⡡⢣⢑⢕⢜⢕⡝\n ⣴⣿⣾⣿⣿⣿⡿⡽⡑⢌⠪⡢⡣⣣⡟\n ⡟⡾⣿⢿⢿⢵⣽⣾⣼⣘⢸⢸⣞⡟\n  ⠁⠇⠡⠩⡫⢿⣝⡻⡮⣒⢽⠋\n\n   no bitches?\n");
//    }
//}