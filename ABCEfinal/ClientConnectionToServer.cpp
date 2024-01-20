#include "MyLibraries.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
#define DEFAULT_BUFLEN 512
#define _OPEN_SYS_SOCK_IPV6


void DoTheThing(bool White, std::unordered_map<uint64_t, std::string> precomputed_moves) {
    WSADATA wsa_data;
    SOCKADDR_IN addr;

    WSAStartup(MAKEWORD(2, 0), &wsa_data);
    const auto server = socket(AF_INET, SOCK_STREAM, 0);

    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(White ? 6969 : 6970);
    connect(server, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr));
    std::cout << "Connected to server!\n";

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int result, time;
    std::string eh, best_move;

    do {
        result = recv(server, recvbuf, recvbuflen, 0);
        std::string message(recvbuf);
        message = message.substr(0, result);
        std::cout << line << " " << message << "\n";


        if (message.length() < 8) {
            std::cout << "An error occured, do you want to continue?\n";
            if (My_analysed_moves != 0)
                std::cout << "STATS\n" <<
                "anal. speed -  " << std::fixed << std::setprecision(2) << (REALtotal_nodes / REALtotal_time / 1000000.0) << "mN/s\n"
                "total nodes - " << std::fixed << std::setprecision(1) << (REALtotal_nodes / 1000000) << " mN\n" <<
                "avrg. position anal. speed - " << std::fixed << std::setprecision(1) << (REALtotal_time / My_analysed_moves * 1.0) << " s\n";
            std::cin >> eh;
            if (eh == "yes") {
                std::cout << "NO I won't";
                return;
            }
            else
                return;
        }
        else if (moveCount < 18 && !precomputed_moves.empty()) {
            chessboard BOARD = initialize_chessboard(message, White, time);
            auto location = precomputed_moves.find(BOARD.hash);
            if (location != precomputed_moves.end()) {
                std::cout << line << "\n";
                best_move = pickAMove(location->second);
                std::cout << "Book move: \n" << best_move << "\n\n";
            }
            else {
                iterativeDepthAnalysis(BOARD, White, DEPTH, best_move, time);
                My_analysed_moves++;
            }
        }
        else {
            //std::vector<std::string> split_fen = split(message, ' ');
            //std::string cut_fen = split_fen[0] + ' ' + split_fen[1] + ' ' + split_fen[2] + ' ' + split_fen[3];
            chessboard BOARD = initialize_chessboard(message, White, time);
            iterativeDepthAnalysis(BOARD, White, DEPTH, best_move, time);
            My_analysed_moves++;
        }


        send(server, best_move.c_str(), best_move.length(), 0);
        moveCount += 2;

    } while (result > 0);
}