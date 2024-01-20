#pragma once
int lazyEVAL(chessboard& BOARD, bool white);
int EVAL(chessboard& BOARD, int depth, bool White, int alpha, int beta);
int EVAL(chessboard& BOARD, int depth, bool White);
int moveValue(MOVE& move, bool white, int pieceValue);