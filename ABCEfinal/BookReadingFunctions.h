#pragma once
std::vector<std::string> split(const std::string line, char split_symbol);
void read_precomputed_moves(std::unordered_map<std::string, std::string>& precomputed_moves, std::string file_name);

//STANDART NOTATION TO MY NOTATION (only usable in oppenings so far)
void veirdMoveToNotVeirdMove(chessboard& BOARD, MOVE& moveOut, std::string veirdMove, bool white);
void writeHashMovesToFile(std::string filePath, std::unordered_map<uint64_t, std::string>& precomputedMovesH);
void readABook(std::string readPathPgn, std::string writePathTxt);
