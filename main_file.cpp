#include "OpenGLChess.h"
#include <iostream>

float speed_x = 0;
float speed_y = 0;

int main(void)
{
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        fprintf(stderr, "Nie można zainicjować GLFW.\n");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_SAMPLES, 8); 

    window = glfwCreateWindow(900, 900, "OpenGL", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Nie można utworzyć okna.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Nie można zainicjować GLEW.\n");
        exit(EXIT_FAILURE);
    }

    glEnable(GL_MULTISAMPLE);

    int samples;
    glGetIntegerv(GL_SAMPLES, &samples);
    std::cout << "MSAA Samples: " << samples << std::endl;

    initOpenGLProgram(window);

    float angle_x = 0;
    float angle_y = 0;
    glfwSetTime(0);
    while (!glfwWindowShouldClose(window)) {
        //angle_x += speed_x * glfwGetTime();
        angle_y += speed_y * glfwGetTime();
        glfwSetTime(0);
        drawScene(window, angle_x, angle_y);
        glfwPollEvents();
    }

    freeOpenGLProgram(window);
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
