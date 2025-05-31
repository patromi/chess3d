#include "OpenGLChess.h"
#include <glm/gtc/matrix_transform.hpp>
#include "constants.h"
#include "lodepng.h"
#include <glm/gtc/type_ptr.hpp>
#include "BoardRenderer.h"

extern float speed_x;
extern float speed_y;
float aspectRatio = 1;
ShaderProgram* sp;
ShaderProgram* spTextured;
GLuint tex0;
GLuint tex1;

extern std::vector<ChessPiece> whitePieces;
extern std::vector<ChessPiece> blackPieces;
extern std::unordered_map<std::string, std::string> pieceMeshMap;
extern Model ChessModel;

extern float angle_x;
extern float angle_y;

namespace {

    void drawBoardSquare(const glm::mat4& M, GLuint texture, ShaderProgram* spTextured) {
        glUniformMatrix4fv(spTextured->u("M"), 1, false, glm::value_ptr(M));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(spTextured->u("texSampler"), 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    void drawChessBoard(glm::mat4 M, float angle_x, float angle_y, ShaderProgram* spTextured, GLuint tex0, GLuint tex1) {
        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < 4; i++) {
                glm::mat4 M1 = glm::mat4(1.0f);
                M1 = glm::scale(M1, glm::vec3(1.0f, 1.0f, 1.0f));
                M1 = glm::rotate(M1, angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
                M1 = glm::rotate(M1, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));
                M1 = glm::translate(M1, glm::vec3(i * -2.0f, 0.0f, j * 2.0f));
                drawBoardSquare(M1, tex0, spTextured);

                glm::mat4 M2 = glm::translate(M1, glm::vec3(-1.0f, 0.0f, 0.0f));
                drawBoardSquare(M2, tex1, spTextured);
            }
            for (int i = 0; i < 4; i++) {
                glm::mat4 M1 = glm::mat4(1.0f);
                M1 = glm::scale(M1, glm::vec3(1.0f, 1.0f, 1.0f));
                M1 = glm::rotate(M1, angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
                M1 = glm::rotate(M1, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));
                M1 = glm::translate(M1, glm::vec3(i * -2.0f, 0.0f, j * 2.0f + 1));
                drawBoardSquare(M1, tex1, spTextured);

                glm::mat4 M2 = glm::translate(M1, glm::vec3(-1.0f, 0.0f, 0.0f));
                drawBoardSquare(M2, tex0, spTextured);
            }
        }
    }

    void drawPiece(const ChessPiece& piece, float angle_x, float angle_y, ShaderProgram* sp, Model& ChessModel, const std::unordered_map<std::string, std::string>& pieceMeshMap) {
        std::string meshName = pieceMeshMap.at(piece.name);
        glm::mat4 pieceModel = glm::mat4(1.0f);
        pieceModel = glm::scale(pieceModel, glm::vec3(0.5f));
        pieceModel = glm::rotate(pieceModel, angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
        pieceModel = glm::rotate(pieceModel, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));
        pieceModel = glm::translate(pieceModel, piece.position);

        glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(pieceModel));
        ChessModel.drawMeshByName(meshName, sp);
    }

    void drawAllPieces(const std::vector<ChessPiece>& pieces, float angle_x, float angle_y, ShaderProgram* sp, Model& ChessModel, const std::unordered_map<std::string, std::string>& pieceMeshMap) {
        for (const auto& piece : pieces) {
            drawPiece(piece, angle_x, angle_y, sp, ChessModel, pieceMeshMap);
        }
    }

} 


GLuint readTexture(const char* filename) {
    GLuint tex;
    std::vector<unsigned char> image;
    unsigned width, height;
    unsigned error = lodepng::decode(image, width, height, filename);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return tex;
}

void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

bool d_was_pressed = false;
bool a_was_pressed = false;
bool r_was_pressed = false;
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) speed_x = -PI / 2;
        if (key == GLFW_KEY_RIGHT) speed_x = PI / 2;
        if (key == GLFW_KEY_UP) speed_y = PI / 2;
        if (key == GLFW_KEY_DOWN) speed_y = -PI / 2;
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
        }

        if (key == GLFW_KEY_D && !d_was_pressed) {
            permission = true;         // Flaga ustawiona tylko raz
            d_was_pressed = true;
        }
        if (key == GLFW_KEY_A && !a_was_pressed) {
            back = true;         // Flaga ustawiona tylko raz
            a_was_pressed = true;
        }
        if (key == GLFW_KEY_R && !r_was_pressed) {
            reset = true;         // Flaga ustawiona tylko raz
            r_was_pressed = true;
        }


    }

    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) speed_x = 0;
        if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) speed_y = 0;
        if (key == GLFW_KEY_D) {
            d_was_pressed = false;
            permission = false;// Odblokuj kolejne kliknięcie
        }
        if (key == GLFW_KEY_A) {
            a_was_pressed = false;
            back = false;// Odblokuj kolejne kliknięcie
        }
        if (key == GLFW_KEY_R) {
            r_was_pressed = false;
            reset = false;// Odblokuj kolejne kliknięcie
        }
    }
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    if (height == 0) return;
    aspectRatio = (float)width / (float)height;
    glViewport(0, 0, width, height);
}

void initOpenGLProgram(GLFWwindow* window) {
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glfwSetWindowSizeCallback(window, windowResizeCallback);
    glfwSetKeyCallback(window, keyCallback);

    sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
    spTextured = new ShaderProgram("v_textured.glsl", NULL, "f_textured.glsl");

    tex0 = readTexture("cell-0.png");
    tex1 = readTexture("cell-1.png");

    ChessModel.loadModel("bishop.obj");
    ChessModel.loadModel("pawn.obj");
    ChessModel.loadModel("rook.obj");
    ChessModel.loadModel("king.obj");
    ChessModel.loadModel("queen.obj");
    ChessModel.loadModel("knight.obj");

    setupChessPieces();
}

void freeOpenGLProgram(GLFWwindow* window) {
    delete sp;
}

void drawBoard(glm::mat4 M, glm::mat4 V, glm::mat4 P, float angle_x, float angle_y) {
    // Ustawienia shaderów i atrybutów
    spTextured->use();
    glUniformMatrix4fv(spTextured->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(spTextured->u("V"), 1, false, glm::value_ptr(V));

    glEnableVertexAttribArray(spTextured->a("vertex"));
    glVertexAttribPointer(spTextured->a("vertex"), 4, GL_FLOAT, false, 0, boardVerts);

    glEnableVertexAttribArray(spTextured->a("texCoord"));
    glVertexAttribPointer(spTextured->a("texCoord"), 2, GL_FLOAT, false, 0, myBoardTexCoords);

    // Rysowanie planszy
    drawChessBoard(M, angle_x, angle_y, spTextured, tex0, tex1);

    glDisableVertexAttribArray(spTextured->a("vertex"));
    glDisableVertexAttribArray(spTextured->a("texCoord"));

    // Rysowanie figur
    sp->use();
    glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

    drawAllPieces(whitePieces, angle_x, angle_y, sp, ChessModel, pieceMeshMap);
    drawAllPieces(blackPieces, angle_x, angle_y, sp, ChessModel, pieceMeshMap);
}

void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 M = glm::mat4(1.0f);
    M = glm::rotate(M, angle_x, glm::vec3(0.0f, 1.0f, 0.0f));
    M = glm::rotate(M, angle_y, glm::vec3(1.0f, 0.0f, 0.0f));

    glm::vec3 cameraPos = glm::vec3(0.0f, 8.0f, -10.0f);
    glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, -0.5f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 V = glm::lookAt(cameraPos, lookAt, up);
    glm::mat4 P = glm::perspective(glm::radians(60.0f), 1.0f, 1.0f, 50.0f);

    drawBoard(M, V, P, angle_x, angle_y, sp, spTextured, tex0, tex1, whitePieces, blackPieces, pieceMeshMap, ChessModel);

    glfwSwapBuffers(window);
}