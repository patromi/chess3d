#pragma once
#include <glm/glm.hpp>
#include "shaderprogram.h"
#include "Model.h"
#include "ChessPieces.h"

extern float boardVerts[];
extern float myBoardTexCoords[];

void drawBoard(
    const glm::mat4& M,
    const glm::mat4& V,
    const glm::mat4& P,
    float angle_x,
    float angle_y,
    ShaderProgram* sp,
    ShaderProgram* spTextured,
    GLuint tex0,
    GLuint tex1,
    GLuint tex2,
    const std::vector<ChessPiece>& whitePieces,
    const std::vector<ChessPiece>& blackPieces,
    const std::unordered_map<std::string, std::string>& pieceMeshMap,
    Model& ChessModel
);
