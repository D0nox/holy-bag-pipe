#pragma once
void doMove(chessboard& BOARD, MOVE& move, bool White, int& capturedPiece, bool& enPassant, int& tempCastling);
void doMoveHash(chessboard& BOARD, MOVE& move, bool White, int& capturedPiece, bool& enPassant, int& tempCastling);
void undoMove(chessboard& BOARD, MOVE& move, bool White, int& capturedPiece, bool& enPassant, int& tempCastling);
void undoMoveHash(chessboard& BOARD, MOVE& move, bool White, int& capturedPiece, bool& enPassant, int& tempCastling);