#include "MyLibraries.h"

bool letterUppercase(char letter) {
    return letter <= 'Z';
}
int mixedCoordinateToInt(std::string sCoordintate) {
    return (sCoordintate[1] - '1') * 8 + 7 - (sCoordintate[0] - 'a');
}


void read_precomputed_moves(std::unordered_map<std::string, std::string>& precomputed_moves, std::string file_name) {
    std::ifstream input_file(file_name);
    if (input_file.is_open()) {
        std::string line;

        while (std::getline(input_file, line)) {
            if (line[0] != '#' && line != "") {
                std::vector<std::string> split_line = split(line, ';');
                precomputed_moves[split_line[0]] = split_line[1];
            }
        }

        input_file.close();
    }
    else {
        std::cout << "ZE \"" << file_name << "\" FILE is MISSING\n";
    }
}
std::vector<std::string> split(const std::string line, char split_symbol) {
    int i = 0;
    std::vector<std::string> words;
    std::string word = "";
    while (i != line.length()) {
        if (line[i] != split_symbol)
            word += line[i];
        else if (word != "") { words.push_back(word); word = ""; }
        i++;
    }
    if (line[line.length() - 1] != split_symbol && word != "")words.push_back(word);
    return words;
}
std::string merge(std::vector<std::string>& splitVector, std::string splitSimbol) {
    std::string edited = "";
    edited = splitVector[0];
    for (int i = 1; i < splitVector.size(); i++) {
        edited += splitSimbol + splitVector[i];
    }
    return edited;
}

std::string pickAMove(std::string moves) {
    std::vector<std::string> spit = split(moves, '|');
    srand(time(0));
    int nr = std::floor((double)std::rand() / RAND_MAX * spit.size());
    return spit[nr];
}

void veirdMoveToNotVeirdMove(chessboard& BOARD, MOVE& moveOut, std::string veirdMove, bool white) {
    veirdMove.erase(std::remove(veirdMove.begin(), veirdMove.end(), '+'), veirdMove.end());

    uint64_t temp;

    if (veirdMove == "O-O") {
        if (white) {
            moveOut.from = e1;
            moveOut.to = g1;
        }
        else {
            moveOut.from = e8;
            moveOut.to = g8;
        }
    }
    else if (veirdMove == "O-O-O") {
        if (white) {
            moveOut.from = e1;
            moveOut.to = b1;
        }
        else {
            moveOut.from = e8;
            moveOut.to = b8;
        }
    }
    else if (letterUppercase(veirdMove[0])) {
        if (veirdMove[0] == 'K' || veirdMove[0] == 'Q') {//both queen and king only exist in one state in the opening
            moveOut.from = find_first_set_bit(veirdMove[0] == 'K' ? getKing(BOARD, white) : getQueen(BOARD, white));
            moveOut.to = mixedCoordinateToInt(veirdMove[1] != 'x' ? veirdMove.substr(1, 2) : veirdMove.substr(2, 2));
        }
        else if (veirdMove[0] == 'B') {
            moveOut.to = mixedCoordinateToInt(veirdMove[1] != 'x' ? veirdMove.substr(1, 2) : veirdMove.substr(2, 2));
            moveOut.from = find_first_set_bit((temp = getBishop(BOARD, white) & get_bishop_attacks(moveOut.to, get_occupancy(BOARD))));
        }
        else if (veirdMove[0] == 'N') {
            if (veirdMove[1] == 'x' || veirdMove.length() == 3) {
                moveOut.to = mixedCoordinateToInt(veirdMove[1] != 'x' ? veirdMove.substr(1, 2) : veirdMove.substr(2, 2));
                moveOut.from = find_first_set_bit((temp = getKnight(BOARD, white) & get_knight_attacks(moveOut.to)));
            }
            else {
                moveOut.to = mixedCoordinateToInt(veirdMove[2] != 'x' ? veirdMove.substr(2, 2) : veirdMove.substr(3, 2));
                if (veirdMove[1] < '9') {
                    moveOut.from = find_first_set_bit((temp = getKnight(BOARD, white) & get_knight_attacks(moveOut.to) & (LINE_AT_THE_BOTTOM << ((veirdMove[1] - '1') * 8))));
                }
                else {
                    moveOut.from = find_first_set_bit((temp = getKnight(BOARD, white) & get_knight_attacks(moveOut.to) & (trim_right << (7 - (veirdMove[0] - 'a')))));
                }
            }
        }
        else if (veirdMove[0] == 'R') {
            if (veirdMove[1] == 'x' || veirdMove.length() == 3) {
                moveOut.to = mixedCoordinateToInt(veirdMove[1] != 'x' ? veirdMove.substr(1, 2) : veirdMove.substr(2, 2));
                moveOut.from = find_first_set_bit((temp = getRook(BOARD, white) & get_rook_attacks(moveOut.to, get_occupancy(BOARD))));
            }
            else std::cout << "CASE " << notImplemented << ":1\n";//to lazy to implement, probs aint gonna need it (2 rooks in one collumn/row)
        }
        else std::cout << "ERROR " << programShouldNotReachThis << "\n";
    }
    else {
        temp = (trim_right << (7 - (veirdMove[0] - 'a'))) & getPawn(BOARD, white);
        if (veirdMove.length() == 2) {
            moveOut.to = mixedCoordinateToInt(veirdMove);
            if (Count_bits(temp) == 1)
                moveOut.from = find_first_set_bit(temp);
            else {
                if (white) {
                    if (get_bit(temp, moveOut.to - 8))
                        moveOut.from = moveOut.to - 8;
                    else
                        moveOut.from = moveOut.to - 16;
                }
                else {
                    if (get_bit(temp, moveOut.to + 8))
                        moveOut.from = moveOut.to + 8;
                    else
                        moveOut.from = moveOut.to + 16;
                }
            } //std::cout << "CASE " << notImplemented << ":2\n";//2 or more pawns in one collumn
        }
        else if (veirdMove.length() == 4) {
            moveOut.to = mixedCoordinateToInt(veirdMove.substr(2, 2));
            if (Count_bits(temp) == 1)
                moveOut.from = find_first_set_bit(temp);
            else {
                if (white && (get_bit(temp, moveOut.to - 7) || get_bit(temp, moveOut.to - 9)))
                    moveOut.from = get_bit(temp, moveOut.to - 7) ? moveOut.to - 7 : moveOut.to - 9;
                else if (!white && (get_bit(temp, moveOut.to + 7) || get_bit(temp, moveOut.to + 9)))
                    moveOut.from = get_bit(temp, moveOut.to + 7) ? moveOut.to + 7 : moveOut.to + 9;
            } //std::cout << "CASE " << notImplemented << ":3\n";//2 or more pawns in one collumn
        }
        else std::cout << "CASE " << notImplemented << ":4\n";//promotion
    }
}

void parseSortOfPgnFileToHashesBook(std::string filePath, std::unordered_map<uint64_t, std::string>& precomputedMovesH) {
    bool white;
    int dontCare;
    MOVE move;

    std::ifstream inputFile(filePath);
    if (inputFile.is_open()) {
        std::string line;
        int g = 0;
        while (std::getline(inputFile, line)) {
            g++;
            if (line[0] != '#' && line != "") {
                chessboard BOARD = initialize_chessboard(POSstarting, white, dontCare);
                std::vector<std::string> split_line = split(line, '.');
                split_line.erase(split_line.begin());
                for (int i = 0; i < split_line.size(); i++) {
                    std::vector<std::string> splitMoves = split(split_line[i], ' ');

                    veirdMoveToNotVeirdMove(BOARD, move, splitMoves[0], white);
                    if (precomputedMovesH.count(BOARD.hash) != 0) {
                        if (precomputedMovesH[BOARD.hash].find(change_to_readable_notation(move, white)) == std::string::npos)
                            precomputedMovesH[BOARD.hash] = precomputedMovesH[BOARD.hash] + "|" + change_to_readable_notation(move, white);
                    }
                    else
                        precomputedMovesH[BOARD.hash] = change_to_readable_notation(move, white);
                    doMove(BOARD, move, white);


                    veirdMoveToNotVeirdMove(BOARD, move, splitMoves[1], !white);
                    if (precomputedMovesH.count(BOARD.hash) != 0) {
                        if (precomputedMovesH[BOARD.hash].find(change_to_readable_notation(move, !white)) == std::string::npos)
                            precomputedMovesH[BOARD.hash] = precomputedMovesH[BOARD.hash] + "|" + change_to_readable_notation(move, !white);
                    }
                    else
                        precomputedMovesH[BOARD.hash] = change_to_readable_notation(move, !white);
                    doMove(BOARD, move, !white);
                }
                //precomputedMovesH[split_line[0]] = split_line[1];
            }
        }
        std::cout << "Finished reading " << filePath << "\n";
        inputFile.close();
    }
    else {
        std::cout << "ZE \"" << pgnBookPath1 << "\" book is MISSING\n";
    }
}
void writeHashMovesToFile(std::string filePath, std::unordered_map<uint64_t, std::string>& precomputedMovesH) {
    std::ofstream outFile(filePath);
    if (outFile.is_open()) {
        for (const auto& pair : precomputedMovesH) {
            outFile << pair.first << ";" << pair.second <<"\n";
        }
        std::cout << "Finished writing at " << filePath << "\n";
        outFile.close();
    }
}
void readHashMovesToUMap(std::string filePath, std::unordered_map<uint64_t, std::string>& precomputedMovesH) {
    std::ifstream input_file(filePath);
    if (input_file.is_open()) {
        std::string line;

        while (std::getline(input_file, line)) {
            if (line[0] != '#' && line != "") {
                std::vector<std::string> split_line = split(line, ';');
                precomputedMovesH[stoull(split_line[0])] = split_line[1];
            }
        }
        std::cout << "Read hash moves at \"" << filePath << "\", total "<<precomputedMovesH.size()<<" positions\n";
        input_file.close();
    }
    else {
        std::cout << "ZE \"" << filePath << "\" FILE is MISSING\n";
    }
}

void readABook(std::string readPathPgn, std::string writePathTxt) {
    std::unordered_map<uint64_t, std::string> precomputedMovesH;

    parseSortOfPgnFileToHashesBook(readPathPgn, precomputedMovesH);
    if(!precomputedMovesH.empty())
        writeHashMovesToFile(writePathTxt, precomputedMovesH);
}