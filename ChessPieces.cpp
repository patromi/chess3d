#include "ChessPieces.h"

std::vector<ChessPiece> whitePieces;
std::vector<ChessPiece> blackPieces;
std::unordered_map<std::string, std::string> pieceMeshMap = {
    {"Pawn", "pawn"},
    {"Rook", "Cylinder"},
    {"Knight", "knight"},
    {"Bishop", "BezierCircle_001"},
    {"Queen", "Cylinder"},
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
    float pieceY = 1.2f;
    for (int i = 0; i < 8; ++i) {
        float x = boardStartX + i * squareSize;
        whitePieces.push_back({ rowOrder[i], {x, pieceY, boardStartZ} });
        whitePieces.push_back({ "Pawn", {x, pieceY, boardStartZ + squareSize} });
        blackPieces.push_back({ rowOrder[i], {x, pieceY, boardStartZ + squareSize * 7} });
        blackPieces.push_back({ "Pawn", {x, pieceY, boardStartZ + squareSize * 6} });
    }
}