#define GLEW_DLL
#define GLFW_DLL

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Header.h"

#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Model.h"
#include "Mesh.h"

float point[] =
{
0.0f, 0.5f, 0.0f,
-0.4f, 0.2f, 0.0f,
-0.4f, -0.4f, 0.0f,
0.4f, -0.4f, 0.0f,
0.4f, 0.2f, 0.0f

};
GLuint index[] = { 0, 1, 2, 3, 4 };

glm::vec3 cameraPosition = glm::vec3(0.0, 0.0, 6.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;
float lastX = SCR_WIDTH / 2;
float lastY = SCR_HEIGHT / 2;

bool firstMouse = true;

float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* MyWindow = glfwCreateWindow(612, 612, "MyWindow", NULL, NULL);

    if (!MyWindow) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(MyWindow);
    glewExperimental = GL_TRUE;

    GLenum ret = glewInit();
    if (GLEW_OK != ret) {
        fprintf(stderr, "ERROR: %s\n", glewGetErrorString(ret));
        return 1;
    }

    SimpleShader shader;
    if (!shader.loadFromFiles("vertex.txt", "fragment.txt")) {
        fprintf(stderr, "ERROR: Failed to load shaders\n");
        return 1;
    }

    Model ourRTC("LR3_CG2.obj");

    GLuint vAo, vBo, Ebo;
    glGenVertexArrays(1, &vAo);
    glGenBuffers(1, &vBo);
    glGenBuffers(1, &Ebo);


    glBindBuffer(GL_ARRAY_BUFFER, vBo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo);
    glBindVertexArray(vAo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glfwSetFramebufferSizeCallback(MyWindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(MyWindow, mouse_callback);
    glfwSetScrollCallback(MyWindow, scroll_callback);
    glfwSetInputMode(MyWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    while (!glfwWindowShouldClose(MyWindow)) {

        glfwPollEvents();
        float timeValue = glfwGetTime();
        float redValue = (sin(timeValue) + 1.0f) / 2.0f;
        float greenValue = (cos(timeValue) + 1.0f) / 2.0f;

        glClearColor(0.8, 0.2, 0.7, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        processInput(MyWindow);
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        glm::mat4 transform = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);

        shader.use();
        shader.setUniform("model", model);
        shader.setUniform("transform", transform);
        shader.setUniform("projection", projection);
        shader.setUniform("view", view);

        shader.setUniform("ourColor", redValue, greenValue, 1.0f, 1.0f);

        glBindVertexArray(vAo);
        //glDrawElements(GL_TRIANGLE_FAN, 5, GL_UNSIGNED_INT, 0);
        if (!ourRTC.meshes.empty()) {
            ourRTC.Draw();
        }
        glfwSwapBuffers(MyWindow);
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    const float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition += cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition -= cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensetivity = 0.1f;
    xoffset *= sensetivity;
    yoffset *= sensetivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
