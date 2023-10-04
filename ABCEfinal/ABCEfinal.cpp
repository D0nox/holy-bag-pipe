#include <iostream>
#include <string>
#include <unordered_map>
#include "ClientConnectionToServer.h"
#include "MoveGen.h"
#include "MoveGenConst.h"
#include "ZobristHashing.h"
#include "MoveGenConst.h"

void startMainProgram() {
    bool white = true;

    std::string colour;
    std::cout << "Are you white or black? (w/b) ";
    std::cin >> colour;
    if (colour == "w")white = true;
    else if ("b" == colour) white = false;
    else {
        std::cout << colour << " aint a colour";
        return;
    }

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

    //do_many_perft_tests("perftsuite.epd");

    #pragma endregion

    #pragma region SinglePerftTests

    //single_perft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4);
    //single_perft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 5);
    //single_perft("r1b1kb1r/pp2pp1p/n2q1n2/2ppP1p1/2B3P1/2N2N2/PPPP1P1P/R1BQK2R w KQkq - 1 7", 1);
    //single_perft("5k2/7P/3P2P1/2N2K2/8/8/PP6/8 w - - 1 68", 1);
    //single_perft("8/2r3p1/p2p4/N2k2p1/8/2P4P/PP4P1/4R1K1 w - - 11 39", 2);
    //single_perft("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1", 5);
    //single_perft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 5);// 4085601 N instead of 4085603 N//e5c4 e2c4
    //single_perft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1pB1P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 1 1", 3);// 84834 N instead of 84835 N
    //single_perft("r3k2r/p2pqpb1/bn2pnp1/2pPN3/1pB1P3/2N2Q1p/PPPB1PPP/R3K2R w KQkq c6 0 2", 2);
    //single_perft("r3k2r/p2pqpb1/bnP1pnp1/4N3/1pB1P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 2", 1);
    //r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0  at depth 4 4085601 N instead of 4085603 N

    #pragma endregion

    #pragma region SinglePositionAnalysisTests

    //test("8/1N6/p3kp2/8/2P5/1P5p/5Kp1/4n3 b - - 1 59", 9);

    #pragma endregion

    return 0;
}
