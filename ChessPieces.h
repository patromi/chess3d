#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include "Model.h"

struct ChessPiece {
    std::string name;
    glm::vec3 position;
};

extern float angle_x;
extern float angle_y;
extern std::vector<ChessPiece> whitePieces;
extern std::vector<ChessPiece> blackPieces;
extern std::unordered_map<std::string, std::string> pieceMeshMap;
extern Model ChessModel;
extern bool flag;
extern bool permission;

void setupChessPieces();
void updatePiecesFromBoard();
void updatePiecesPositions();