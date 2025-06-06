﻿#include <array>
#include <vector>
#include "ChessPieces.h"
#include "PGN.h"
#include <thread>
#include <map>

#include <chrono>
extern char board[8][8];
std::vector<ChessPiece> whitePieces;
std::vector<ChessPiece> blackPieces;
std::vector<std::vector<ChessPiece>> whiteHistory;
std::vector<std::vector<ChessPiece>> blackHistory;
std::unordered_map<std::string, std::string> pieceMeshMap = {
    {"Pawn", "pawn"},
    {"Rook", "Rook"},
    {"Knight", "knight"},
    {"Bishop", "BezierCircle_001"},
    {"Queen", "Queen"},
    {"King", "Cube_002"}
};
Model ChessModel;

std::vector<std::string> rowOrder = {
    "Rook", "Knight", "Bishop", "Queen",  "King","Bishop", "Knight", "Rook"
};

void setupChessPieces() {
    float boardStartX = -8.0f;
    float boardStartZ = -11.0f;
    float squareSize = 2.0f;
    float pieceY = 1.0f;

    for (int i = 0; i < 8; ++i) {
        float x = boardStartX + i * squareSize;
        std::string pieceType = rowOrder[i];

        float knightRotation = 0.0f;
        if (pieceType == "Knight") {
            knightRotation = 90.0f;
        }

        
        blackPieces.push_back({ pieceType, {x, pieceY, boardStartZ}, knightRotation, "black" });
        blackPieces.push_back({ "Pawn", {x, pieceY, boardStartZ + squareSize}, 0.0f, "black" });

        
        whitePieces.push_back({ pieceType, {x, pieceY, boardStartZ + squareSize * 7}, knightRotation, "white" });
        whitePieces.push_back({ "Pawn", {x, pieceY, boardStartZ + squareSize * 6}, 0.0f, "white" });
    }
   }

char previousBoard[8][8];

void savePreviousBoardSnapshot() {
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            previousBoard[i][j] = board[i][j];
}
void updatePiecesFromBoard() {
    std::map<char, std::string> pieceMap = {
        {'K', "King"}, {'Q', "Queen"}, {'R', "Rook"},
        {'B', "Bishop"}, {'N', "Knight"}, {'P', "Pawn"},
        {'k', "King"}, {'q', "Queen"}, {'r', "Rook"},
        {'b', "Bishop"}, {'n', "Knight"}, {'p', "Pawn"}
    };

    float boardStartX = -8.0f;
    float boardStartZ = -11.0f;
    float squareSize = 2.0f;
    float pieceY = 1.0f;

    std::vector<bool> whiteUsed(whitePieces.size(), false);
    std::vector<bool> blackUsed(blackPieces.size(), false);

    auto getPieceAt = [](const std::vector<ChessPiece>& pieces, glm::vec3 position) -> int {
        for (size_t i = 0; i < pieces.size(); ++i) {
            if (glm::distance(pieces[i].position, position) < 0.01f) {
                return i;
            }
        }
        return -1;
        };

    for (auto& p : whitePieces) p.is_moved = false;
    for (auto& p : blackPieces) p.is_moved = false;

    int lastMovedRow = -1, lastMovedCol = -1;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (previousBoard[row][col] != board[row][col]) {
                if (board[row][col] != '.' && previousBoard[row][col] == '.') {
                    lastMovedRow = row;
                    lastMovedCol = col;
                    break;
                }
            }
        }
        if (lastMovedRow != -1) break;
    }

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            char symbol = board[row][col];
            if (symbol == '.') continue;

            bool isWhite = std::isupper(symbol);
            std::string type = pieceMap[symbol];

            float x = boardStartX + col * squareSize;
            float z = boardStartZ + row * squareSize;
            glm::vec3 newPos = glm::vec3(x, pieceY, z);

            auto& pieces = isWhite ? whitePieces : blackPieces;
            auto& usedFlags = isWhite ? whiteUsed : blackUsed;
            auto& enemyPieces = isWhite ? blackPieces : whitePieces;
            auto& enemyUsed = isWhite ? blackUsed : whiteUsed;

            int enemyIndex = getPieceAt(enemyPieces, newPos);
            if (enemyIndex != -1) {
                enemyPieces.erase(enemyPieces.begin() + enemyIndex);
                enemyUsed.erase(enemyUsed.begin() + enemyIndex);
            }

            for (size_t i = 0; i < pieces.size(); ++i) {
                if (!usedFlags[i] && pieces[i].name == type) {
                    pieces[i].position = newPos;

                    if (row == lastMovedRow && col == lastMovedCol) {
                        pieces[i].is_moved = true;
                    }

                    usedFlags[i] = true;
                    break;
                }
            }
        }
    }

    auto cleanUnused = [](std::vector<ChessPiece>& pieces, std::vector<bool>& used) {
        for (int i = pieces.size() - 1; i >= 0; --i) {
            if (!used[i]) {
                pieces.erase(pieces.begin() + i);
                used.erase(used.begin() + i);
            }
        }
        };
    cleanUnused(whitePieces, whiteUsed);
    cleanUnused(blackPieces, blackUsed);
}

bool permission = false;
bool back = false;
bool reset = false;
extern char board[8][8];
std::vector<std::array<std::array<char, 8>, 8>> boardHistory;
int currentBoardIndex = -1;

void saveCurrentBoardToHistory() {
    std::array<std::array<char, 8>, 8> snapshot;
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            snapshot[i][j] = board[i][j];

    if (currentBoardIndex + 1 < (int)boardHistory.size()) {
        boardHistory.resize(currentBoardIndex + 1);
        whiteHistory.resize(currentBoardIndex + 1);
        blackHistory.resize(currentBoardIndex + 1);
    }

    boardHistory.push_back(snapshot);
    whiteHistory.push_back(whitePieces);
    blackHistory.push_back(blackPieces);
    currentBoardIndex++;
}

void loadBoardFromHistory(int index) {
    if (index >= 0 && index < (int)boardHistory.size()) {
        const auto& snapshot = boardHistory[index];
        for (int i = 0; i < 8; ++i)
            for (int j = 0; j < 8; ++j)
                board[i][j] = snapshot[i][j];

        whitePieces = whiteHistory[index];
        blackPieces = blackHistory[index];
    }
}
void updatePiecesPositions() {
    savePreviousBoardSnapshot();

    if (!permission && back) {
        if (currentBoardIndex < 0) {
            std::cout << "End of history, u cannot go more back.\n";
            back = false;
            return;
        }
        if (currentBoardIndex == 0) {
            initBoard();
            updatePiecesFromBoard();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            currentBoardIndex--;
            back = false;
            return;
        }
        currentBoardIndex--;
        loadBoardFromHistory(currentBoardIndex);
        updatePiecesFromBoard();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        back = false;
        return;
    }

    if (permission) {
        if (currentBoardIndex + 1 < (int)boardHistory.size()) {
            currentBoardIndex++;
            loadBoardFromHistory(currentBoardIndex);
        }
        else {
            readPGN(permission);
            saveCurrentBoardToHistory();
        }

        updatePiecesFromBoard();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        permission = false;
    }

    if (reset == true) {
        initBoard();
        updatePiecesFromBoard();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        whitePieces = whiteHistory[0];
        blackPieces = blackHistory[0];
        currentBoardIndex = -1;
        reset = false;

    }
}


    








