#include <array> // Make sure this is at the top!
#include <vector>
#include "ChessPieces.h"
#include "PGN.h"
#include <thread>
#include <map> // Add this include to define std::map

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
    "Rook", "Knight", "Bishop", "Queen", "King", "Bishop", "Knight", "Rook"
};




void setupChessPieces() {
    float boardStartX = -8.0f;
    float boardStartZ = -11.0f;
    float squareSize = 2.0f;
    float pieceY = 1.0f;
    for (int i = 0; i < 8; ++i) {
        float x = boardStartX + i * squareSize;
        whitePieces.push_back({ rowOrder[i], {x, pieceY, boardStartZ} });
        whitePieces.push_back({ "Pawn", {x, pieceY, boardStartZ + squareSize} });
        blackPieces.push_back({ rowOrder[i], {x, pieceY, boardStartZ + squareSize * 7} });
        blackPieces.push_back({ "Pawn", {x, pieceY, boardStartZ + squareSize * 6} });
    }
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

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            char symbol = board[row][col];
            if (symbol == '.') continue;

            bool isWhite = std::isupper(symbol);
            std::string type = pieceMap[symbol];

            float x = boardStartX + col * squareSize;
            float z = boardStartZ + (7 - row) * squareSize;
            glm::vec3 newPos = glm::vec3(x, pieceY, z);

            auto& pieces = isWhite ? whitePieces : blackPieces;
            auto& usedFlags = isWhite ? whiteUsed : blackUsed;
            auto& enemyPieces = isWhite ? blackPieces : whitePieces;
            auto& enemyUsed = isWhite ? blackUsed : whiteUsed;

            // Obsługa bicia – sprawdź, czy na pozycji jest figura przeciwnika
            int enemyIndex = getPieceAt(enemyPieces, newPos);
            if (enemyIndex != -1) {
                enemyPieces.erase(enemyPieces.begin() + enemyIndex);
                enemyUsed.erase(enemyUsed.begin() + enemyIndex);
            }

            // Znajdź figurę tego typu która jeszcze nie została użyta
            for (size_t i = 0; i < pieces.size(); ++i) {
                if (!usedFlags[i] && pieces[i].name == type) {
                    pieces[i].position = newPos;
                    usedFlags[i] = true;
                    break;
                }
            }
        }
    }

    // Opcjonalnie: usuń niewykorzystane figury (czyli te, które zniknęły z planszy)
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

    // Ucinanie przyszłości
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
    // ⬅️ Ruch do tyłu
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

    // ➡️ Nowy ruch do przodu
    if (permission) {
        // Jeśli mamy jeszcze historię do przodu – użyj jej
        if (currentBoardIndex + 1 < (int)boardHistory.size()) {
            currentBoardIndex++;
            loadBoardFromHistory(currentBoardIndex);
        }
        else {
            readPGN(permission);                 // wykonaj ruch
            saveCurrentBoardToHistory();        // zapisz aktualny board
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


    








