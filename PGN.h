#pragma once
#include <string>
#include <utility>

void initBoard();
void printBoard();
bool isPiece(char c, char type, bool white);
bool isValidDirection(char type, int r, int c, int toRow, int toCol, bool white);
std::pair<int, int> findMatchingPiece(char type, bool white, int toRow, int toCol, char disambigFile = 0, char disambigRank = 0);
void movePiece(const std::string& move, bool whiteToMove);
std::string indexToSquare(int row, int col);
void readPGN(bool permission);