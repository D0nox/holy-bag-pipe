#pragma once
int EVAL(chessboard& BOARD, int depth, bool White);
int quickEVAL(chessboard& BOARD);
int moveValue(MOVE& move, bool white, int pieceValue);