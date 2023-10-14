#include <iostream>
#include <string>
#include <unordered_map>
#include "ClientConnectionToServer.h"
#include "MoveGen.h"
#include "MoveGenConst.h"
#include "ZobristHashing.h"
#include "MoveGenConst.h"
#include "DebugFunctions.h"

void startMainProgram() {
    bool white = true;

    /*std::string colour;
    std::cout << "Are you white or black? (w/b) ";
    std::cin >> colour;
    if (colour == "w")white = true;
    else if ("b" == colour) white = false;
    else {
        std::cout << colour << " aint a colour";
        return;
    }*/
    std::unordered_map<std::string, std::string> precomputed_moves;
    read_precomputed_moves(precomputed_moves, PrecomputedMovePath);

    DoTheThing(white, precomputed_moves);
}
int main()
{
    init_sliders_attacks(rook);
    init_sliders_attacks(bishop);
    
    generate_hashes_for_board_hashing();

    startMainProgram();


    #pragma region ALotOfPerftTests

    //do_many_perft_tests("perftsuite.epd");//not working currently due to changed node counting but idc

    #pragma endregion

    #pragma region SinglePerftTests

    //single_perft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 6);
    //single_perft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 5);
    //single_perft("r1b1kb1r/pp2pp1p/n2q1n2/2ppP1p1/2B3P1/2N2N2/PPPP1P1P/R1BQK2R w KQkq - 1 7", 1);
    //single_perft("5k2/7P/3P2P1/2N2K2/8/8/PP6/8 w - - 1 68", 1);
    //single_perft("8/2r3p1/p2p4/N2k2p1/8/2P4P/PP4P1/4R1K1 w - - 11 39", 2);
    //single_perft("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1", 5);
    //single_perft("rnbqkb1r/ppp2ppp/4pn2/3p4/8/P1N2N2/1PPPPPPP/R1BQKB1R w KQkq d6 0 4", 1);

    #pragma endregion

    #pragma region SinglePositionAnalysisTests

    //test("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 8);
    //test("2r1k3/pp1b1pp1/n7/3R4/8/N1P1P3/PP3rPP/2K3NR w - - 0 20", 8);//a3h5 g1h3
    //test("8/1N6/p3kp2/8/2P5/1P5p/5Kp1/4n3 b - - 1 59", 9);
    //test("rnbqkb1r/ppp2ppp/4pn2/3p4/8/P1N2N2/1PPPPPPP/R1BQKB1R w KQkq d6 0 4", 2);
    //test("3rkbn1/pp4p1/7r/P3pp2/RpP3p1/4P1P1/1PK1NP1P/1N5R w - f6 0 16", 2);

    #pragma endregion

    return 0;
}
