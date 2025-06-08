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
GLuint tex2;

extern std::vector<ChessPiece> whitePieces;
extern std::vector<ChessPiece> blackPieces;
extern std::unordered_map<std::string, std::string> pieceMeshMap;
extern Model ChessModel;

extern float angle_x;
extern float angle_y;

GLuint readTexture(const char* filename) {
    GLuint tex;
    std::vector<unsigned char> image;
    unsigned width, height;

    unsigned error = lodepng::decode(image, width, height, filename);
    if (error) {
        std::cerr << "Texture loading error (" << filename << "): " << lodepng_error_text(error) << std::endl;
        return 0;
    }

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, image.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);

    GLfloat maxAniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);

    glBindTexture(GL_TEXTURE_2D, 0);

    return tex;
}

void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

bool d_was_pressed = false;
bool a_was_pressed = false;
bool r_was_pressed = false;
int cameraMode = 0; 
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) speed_x = -PI / 2;
        if (key == GLFW_KEY_RIGHT) speed_x = PI / 2;
        if (key == GLFW_KEY_UP) speed_y = PI / 2;
        if (key == GLFW_KEY_DOWN) speed_y = -PI / 2;
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
        }
        if (key == GLFW_KEY_1 && !d_was_pressed) {
            tex0 = readTexture("texture/cell-0.png");
            tex1 = readTexture("texture/cell-1.png");;
        }
        if (key == GLFW_KEY_2 && !d_was_pressed) {
            tex0 = readTexture("texture/porcelain1.png");
            tex1 = readTexture("texture/porcelain2.png");
        }
        if (key == GLFW_KEY_3 && !d_was_pressed) {
            tex0 = readTexture("texture/carbon2.png");
            tex1 = readTexture("texture/carbon1.png");
        }
        if (key == GLFW_KEY_D && !d_was_pressed) {
            permission = true;
            d_was_pressed = true;
        }
        if (key == GLFW_KEY_A && !a_was_pressed) {
            back = true;         
            a_was_pressed = true;
        }
        if (key == GLFW_KEY_R && !r_was_pressed) {
            reset = true;         
            r_was_pressed = true;
        }
        if (key == GLFW_KEY_B){
			std::cout << cameraMode << std::endl;
            cameraMode = 0;
        }
        if (key == GLFW_KEY_N) {
            cameraMode = 1;
        }
        if (key == GLFW_KEY_M) {
            cameraMode = 2;
        }
    }

    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) speed_x = 0;
        if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) speed_y = 0;
        if (key == GLFW_KEY_D) {
            d_was_pressed = false;
            permission = false;
        }
        if (key == GLFW_KEY_A) {
            a_was_pressed = false;
            back = false;
        }
        if (key == GLFW_KEY_R) {
            r_was_pressed = false;
            reset = false;
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



    sp = new ShaderProgram("v_simplest2.glsl", NULL, "f_simplest2.glsl");
	spTextured = new ShaderProgram("v_simplest1.glsl", NULL, "f_simplest1.glsl");
    
    tex0 = readTexture("texture/cell-0.png");
    tex1 = readTexture("texture/cell-1.png");
    tex2 = readTexture("texture/cell_S.png");

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

void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
    
    if (cameraMode == 2) {
        glClearColor(0.575f, 0.65f, 0.725f, 1.0f);
    }
	else if (cameraMode == 1) {
        glClearColor(0.35f, 0.4f, 0.45f, 1.0f);
	}
    else {
        glClearColor(0.8f, 0.9f, 1.0f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 M = glm::mat4(1.0f);

    glm::vec3 cameraPos;
    glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    float fov = 45.0f;
    glm::vec3 center = glm::vec3(-0.5f, 0.0f, -2.0f);

    if (cameraMode == 0) {
        glm::vec3 raisedCenter = center + glm::vec3(0.0f, 2.0f, 0.0f);
        cameraPos = raisedCenter + glm::vec3(0.0f, 10.0f, 13.0f);
        lookAt = raisedCenter;
    }
    else if (cameraMode == 1) {
        glm::vec3 raisedCenter = center + glm::vec3(0.0f, 2.0f, 0.0f);
        cameraPos = raisedCenter + glm::vec3(0.0f, 10.0f, -13.0f);
        lookAt = raisedCenter;
    }
    else if (cameraMode == 2) {
        cameraPos = center + glm::vec3(0.0f, 22.0f, 0.001f);
        lookAt = center;
        up = glm::vec3(0.0f, 0.0f, -1.0f);
        fov = 35.0f;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = (float)width / (float)height;
    static int frameCounter = 0;
    frameCounter++;

    glm::mat4 V = glm::lookAt(cameraPos, lookAt, up);
    glm::mat4 P = glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);

    drawBoard(M, V, P, angle_x, angle_y, sp, spTextured, tex0, tex1, tex2,
        whitePieces, blackPieces, pieceMeshMap, ChessModel);

    glfwSwapBuffers(window);
}
