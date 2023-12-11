#pragma once
//no position info loss (except en passant and stable pos)
void doMove(chessboard& BOARD, MOVE& move, bool White, int& capturedPiece, bool& enPassant, int& tempCastling);
//no position info loss (except en passant and stable pos)
void undoMove(chessboard& BOARD, MOVE& move, bool White, int& capturedPiece, bool& enPassant, int& tempCastling);
//position info loss
void doMove(chessboard& BOARD, MOVE& move, bool White);
//only execute the move on the hash
void doMoveHash(chessboard& BOARD, MOVE& move, bool White, int& capturedPiece, bool& enPassant, int& tempCastling);
//only unexecute the move on the hash
void undoMoveHash(chessboard& BOARD, MOVE& move, bool White, int& capturedPiece, bool& enPassant, int& tempCastling);