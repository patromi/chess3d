#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "shaderprogram.h"
#include "Model.h"
#include "ChessPieces.h"

extern float angle_x; // Rotation angle around the x-axis
extern float angle_y; // Rotation angle around the y-axiss
void error_callback(int error, const char* description);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void windowResizeCallback(GLFWwindow* window, int width, int height);
void initOpenGLProgram(GLFWwindow* window);
void freeOpenGLProgram(GLFWwindow* window);
void drawScene(GLFWwindow* window, float angle_x, float angle_y);