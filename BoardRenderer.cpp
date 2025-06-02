#include "BoardRenderer.h"
#include <glm/gtc/type_ptr.hpp>
#include "myCube.h"
#include "myTeapot.h"
#include "myBoard.h"


namespace {

void setBoardAttributes(ShaderProgram* spTextured) {
    glEnableVertexAttribArray(spTextured->a("vertex"));
    glVertexAttribPointer(spTextured->a("vertex"), 4, GL_FLOAT, false, 0, boardVerts);

    glEnableVertexAttribArray(spTextured->a("texCoord0"));
    glVertexAttribPointer(spTextured->a("texCoord0"), 2, GL_FLOAT, false, 0, myBoardTexCoords);
}

void unsetBoardAttributes(ShaderProgram* spTextured) {
    glDisableVertexAttribArray(spTextured->a("vertex"));
    glDisableVertexAttribArray(spTextured->a("texCoord0"));
}

glm::mat4 getTransformedMatrix(float scale, float angle_x, float angle_y, glm::vec3 translate) {
    glm::mat4 M = glm::mat4(1.0f);
    M = glm::scale(M, glm::vec3(scale));
    M = glm::rotate(M, angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
    M = glm::rotate(M, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));
    M = glm::translate(M, translate);
    return M;
}

void drawBoardSquare(const glm::mat4& M, GLuint texture, ShaderProgram* spTextured) {
    glUniformMatrix4fv(spTextured->u("M"), 1, false, glm::value_ptr(M));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(spTextured->u("textureMap0"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawChessBoard(float angle_x, float angle_y, ShaderProgram* spTextured, GLuint tex0, GLuint tex1) {
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            glm::mat4 M1 = getTransformedMatrix(1.0f, angle_x, angle_y, glm::vec3(i * -2.0f, 0.0f, j * 2.0f));
            drawBoardSquare(M1, tex0, spTextured);

            glm::mat4 M2 = glm::translate(M1, glm::vec3(-1.0f, 0.0f, 0.0f));
            drawBoardSquare(M2, tex1, spTextured);
        }
        for (int i = 0; i < 4; i++) {
            glm::mat4 M1 = getTransformedMatrix(1.0f, angle_x, angle_y, glm::vec3(i * -2.0f, 0.0f, j * 2.0f + 1));
            drawBoardSquare(M1, tex1, spTextured);

            glm::mat4 M2 = glm::translate(M1, glm::vec3(-1.0f, 0.0f, 0.0f));
            drawBoardSquare(M2, tex0, spTextured);
        }
    }
}

void drawPiece(const ChessPiece& piece, float angle_x, float angle_y, ShaderProgram* sp, Model& ChessModel, const std::unordered_map<std::string, std::string>& pieceMeshMap, GLuint tex0, GLuint tex1) {
    std::string meshName = pieceMeshMap.at(piece.name);

    glm::mat4 pieceModel = getTransformedMatrix(0.5f, angle_x, angle_y, piece.position);
	pieceModel = glm::scale(pieceModel, glm::vec3(0.9f));

    if (piece.name == "Knight") {
        if (piece.color == "white") {
            pieceModel = glm::rotate(pieceModel, glm::radians(270.0f), glm::vec3(0, 1, 0));
        }
        else {
            pieceModel = glm::rotate(pieceModel, glm::radians(90.0f), glm::vec3(0, 1, 0));
        }
    }

    
    if (piece.color == "white") {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex1);
        glUniform1i(sp->u("textureMap0"), 0);
    }
    else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex0);
        glUniform1i(sp->u("textureMap0"), 0);
    }

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(pieceModel));
    glUniform1i(sp->u("textureMap0"), 0);

    ChessModel.drawMeshByName(meshName, sp);
}

void drawAllPieces(const std::vector<ChessPiece>& pieces, float angle_x, float angle_y, ShaderProgram* sp, Model& ChessModel, const std::unordered_map<std::string, std::string>& pieceMeshMap, GLuint tex0, GLuint tex1) {
    for (const auto& piece : pieces) {
        drawPiece(piece, angle_x, angle_y, sp, ChessModel, pieceMeshMap, tex0, tex1);
    }
}

} 

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
    const std::vector<ChessPiece>& whitePieces,
    const std::vector<ChessPiece>& blackPieces,
    const std::unordered_map<std::string, std::string>& pieceMeshMap,
    Model& ChessModel
) {
    spTextured->use();
    glUniformMatrix4fv(spTextured->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(spTextured->u("V"), 1, false, glm::value_ptr(V));
    setBoardAttributes(spTextured);

    drawChessBoard(angle_x, angle_y, spTextured, tex0, tex1);

    unsetBoardAttributes(spTextured);

    sp->use();
    glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

    updatePiecesPositions();
    drawAllPieces(whitePieces, angle_x, angle_y, sp, ChessModel, pieceMeshMap, tex0, tex1);
    drawAllPieces(blackPieces, angle_x, angle_y, sp, ChessModel, pieceMeshMap, tex0, tex1);
}

