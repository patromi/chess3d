#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include "Model.h"

struct ChessPiece {
    std::string name;
    glm::vec3 position;
    float rotation = 0.0f;  
    std::string color;
	bool is_moved = false;  // Indicates if the piece has been moved

    ChessPiece(const std::string& name, const glm::vec3& position, float rotation, const std::string& color)
        : name(name), position(position), rotation(rotation), color(color) {}
};

extern float angle_x;
extern float angle_y;
extern std::vector<ChessPiece> whitePieces;
extern std::vector<ChessPiece> blackPieces;
extern std::unordered_map<std::string, std::string> pieceMeshMap;
extern Model ChessModel;
extern bool flag;
extern bool permission;
extern bool back;
extern bool reset;
void setupChessPieces();
void updatePiecesFromBoard();
void updatePiecesPositions();