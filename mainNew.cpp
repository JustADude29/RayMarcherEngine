#include <Eigen/Dense>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "src/glm/glm.hpp"

#include <iostream>

void framebuffer_size_callback(GLFWwindow*, int, int);
void processInput(GLFWwindow*);
void mouse_callback(GLFWwindow*, double, double);
void mouse_button_callback(GLFWwindow*, int, int, int);
void scroll_callback(GLFWwindow*, double, double);

unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

float delta_time = 0.0f;
float last_frame_time = 0.0f;

bool first_mouse = true;
bool mouse_hidden = true;
float last_x = SCR_WIDTH / 2.0f;
float last_y = SCR_WIDTH / 2.0f;

float fps_sum = 0.0f;
int fps_steps = 10;
int fps_steps_counter = 0;

int main(int argc, const char * argv[]){
    if(!glfwInit()){
        std::cerr<<"GLFW INITIALIZATION FAILED"<<std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Raymarcher", NULL, NULL);

    if(window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr<<"FAILED TO LOAD GLAD"<<std::endl;
        return -1;
    }

    glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

