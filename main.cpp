#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <glm/gtc/constants.hpp>



#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"

const unsigned int width = 1280;
const unsigned int height = 960;

// Interactive Camera Parameters
float cameraRadius = 45.0f;  // Zoom distance
float yaw = -90.0f;          // Horizontal angle (degrees)
float pitch = 30.0f;         // Vertical angle (degrees)
float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool isDragging = false;
bool firstMouse = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Mouse Button Callback (Detect Left Click Drag)
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isDragging = true;
            firstMouse = true;
        }
        else if (action == GLFW_RELEASE) {
            isDragging = false;
        }
    }
}

// Cursor Position Callback (Rotate Scene on Drag)
void cursor_position_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (!isDragging) return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed: y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.25f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Clamp pitch to avoid flipping over or going below ground level
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < 1.0f)  pitch = 1.0f;
}

// Scroll Callback (Zoom In / Zoom Out)
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    cameraRadius -= static_cast<float>(yoffset) * 3.0f;
    if (cameraRadius < 5.0f)   cameraRadius = 5.0f;   // Maximum zoom-in limit
    if (cameraRadius > 120.0f) cameraRadius = 120.0f; // Maximum zoom-out limit
}

// Keyboard Input Processing
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Keyboard rotation & height controls directly adjust camera angles/radius
    float rotateSpeed = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        yaw -= rotateSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        yaw += rotateSpeed;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        pitch = std::min(pitch + rotateSpeed, 89.0f);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        pitch = std::max(pitch - rotateSpeed, 1.0f);
}

// Cube vertices (Positions + Normals)
GLfloat vertices[] = {
    // Back
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    // Front
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    // Left
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    // Right
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     // Bottom
     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
      0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     // Top
     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

void drawCube(Shader& shaderProgram, VAO& vao, glm::mat4 model, glm::vec3 color) {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "objectColor"), color.x, color.y, color.z);

    vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
}


void drawSemiOval(Shader& shaderProgram, float rx, float h, float rz, glm::vec3 pos, float rotationY, glm::vec3 color) {
    static GLuint semiVAO = 0;
    static GLsizei vertexCount = 0;

    // Initialize geometry once on the first run
    if (semiVAO == 0) {
        const int SEGMENTS = 20;
        std::vector<GLfloat> vertices;

        for (int i = 0; i < SEGMENTS; i++) {
            float theta1 = -glm::pi<float>() / 2.0f + (glm::pi<float>() * i / SEGMENTS);
            float theta2 = -glm::pi<float>() / 2.0f + (glm::pi<float>() * (i + 1) / SEGMENTS);

            float x1 = std::cos(theta1), z1 = std::sin(theta1);
            float x2 = std::cos(theta2), z2 = std::sin(theta2);

            // 1. Outer Wall Triangles (Normals outward)
            vertices.insert(vertices.end(), {
                x1, -0.5f, z1,  x1, 0.0f, z1,
                x2, -0.5f, z2,  x2, 0.0f, z2,
                x1,  0.5f, z1,  x1, 0.0f, z1,

                x1,  0.5f, z1,  x1, 0.0f, z1,
                x2, -0.5f, z2,  x2, 0.0f, z2,
                x2,  0.5f, z2,  x2, 0.0f, z2
                });

            // 2. Top Cap Triangles (Normal = +Y)
            vertices.insert(vertices.end(), {
                0.0f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
                x1,   0.5f, z1,    0.0f, 1.0f, 0.0f,
                x2,   0.5f, z2,    0.0f, 1.0f, 0.0f
                });

            // 3. Bottom Cap Triangles (Normal = -Y)
            vertices.insert(vertices.end(), {
                0.0f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f,
                x2,   -0.5f, z2,    0.0f, -1.0f, 0.0f,
                x1,   -0.5f, z1,    0.0f, -1.0f, 0.0f
                });
        }

        // 4. Flat Back Wall (Normal = -X)
        vertices.insert(vertices.end(), {
            0.0f, -0.5f, -1.0f,  -1.0f, 0.0f, 0.0f,
            0.0f, -0.5f,  1.0f,  -1.0f, 0.0f, 0.0f,
            0.0f,  0.5f, -1.0f,  -1.0f, 0.0f, 0.0f,

            0.0f,  0.5f, -1.0f,  -1.0f, 0.0f, 0.0f,
            0.0f, -0.5f,  1.0f,  -1.0f, 0.0f, 0.0f,
            0.0f,  0.5f,  1.0f,  -1.0f, 0.0f, 0.0f
            });

        vertexCount = static_cast<GLsizei>(vertices.size() / 6);

        GLuint semiVBO;
        glGenVertexArrays(1, &semiVAO);
        glGenBuffers(1, &semiVBO);

        glBindVertexArray(semiVAO);
        glBindBuffer(GL_ARRAY_BUFFER, semiVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Render pass
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(rx, h, rz));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "objectColor"), color.x, color.y, color.z);

    glBindVertexArray(semiVAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}


void drawBuildingBlock(Shader& shaderProgram, VAO& vao, float xOffset, float zOffset) {
    glm::vec3 mainColor(0.93f, 0.88f, 0.80f);
    glm::vec3 balconyColor(0.75f, 0.65f, 0.55f);

    // 1. Main Building Volume
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(xOffset, 4.0f, zOffset));
    model = glm::scale(model, glm::vec3(8.0f, 8.0f, 4.0f));
    drawCube(shaderProgram, vao, model, mainColor);

    // Balcony dimensions
    float balconyOutwardRadius = 1.5f; // Extension along X-axis
    float balconyHeight = 0.2f;        // Thickness in Y-axis
    float balconyDepthRadius = 1.8f;   // Radius along Z-axis (fits within building depth 4.0)

    // 2. Horizontal Half-Oval Balconies (5 floors)
    for (int i = 1; i <= 5; i++) {
        float yPos = i * 1.3f;

        // +X Side (Right Face) Balcony facing +X (0 degrees rotation)
        glm::vec3 posPlusX(xOffset + 3.0f, yPos, zOffset);
        drawSemiOval(shaderProgram, balconyOutwardRadius, balconyHeight, balconyDepthRadius, posPlusX, 0.0f, balconyColor);

        // -X Side (Left Face) Balcony facing -X (180 degrees rotation)
        glm::vec3 posMinusX(xOffset - 3.0f, yPos, zOffset);
        drawSemiOval(shaderProgram, balconyOutwardRadius, balconyHeight, balconyDepthRadius, posMinusX, 180.0f, balconyColor);
    }
}

void drawFourConnectedBlocks(Shader& shaderProgram, VAO& vao, float xOffset) {
    float zPositions[] = { -9.0f, -3.0f, 3.0f, 9.0f };
    glm::vec3 corridorColor(0.93f, 0.88f, 0.80f);

    for (int i = 0; i < 4; i++) {
        drawBuildingBlock(shaderProgram, vao, xOffset, zPositions[i]);

        if (i < 3) {
            float midZ = (zPositions[i] + zPositions[i + 1]) / 2.0f;

            glm::mat4 connModel = glm::mat4(1.0f);
            connModel = glm::translate(connModel, glm::vec3(xOffset, 3.0f, midZ));
            connModel = glm::scale(connModel, glm::vec3(2.0f, 8.0f, 8.0f));
            drawCube(shaderProgram, vao, connModel, corridorColor);
        }
    }
}

void drawCourtyardBuilding(Shader& shader, VAO& vao, float x, float z, float w, float d, float h, float wallThick, glm::vec3 color) {
    glm::mat4 m1 = glm::translate(glm::mat4(1.0f), glm::vec3(x, h / 2.0f, z - d / 2.0f + wallThick / 2.0f));
    m1 = glm::scale(m1, glm::vec3(w, h, wallThick));
    drawCube(shader, vao, m1, color);

    glm::mat4 m2 = glm::translate(glm::mat4(1.0f), glm::vec3(x, h / 2.0f, z + d / 2.0f - wallThick / 2.0f));
    m2 = glm::scale(m2, glm::vec3(w, h, wallThick));
    drawCube(shader, vao, m2, color);

    glm::mat4 m3 = glm::translate(glm::mat4(1.0f), glm::vec3(x - w / 2.0f + wallThick / 2.0f, h / 2.0f, z));
    m3 = glm::scale(m3, glm::vec3(wallThick, h, d - 2.0f * wallThick));
    drawCube(shader, vao, m3, color);

    glm::mat4 m4 = glm::translate(glm::mat4(1.0f), glm::vec3(x + w / 2.0f - wallThick / 2.0f, h / 2.0f, z));
    m4 = glm::scale(m4, glm::vec3(wallThick, h, d - 2.0f * wallThick));
    drawCube(shader, vao, m4, color);
}

void drawRoads(Shader& shaderProgram, VAO& vao) {
    glm::vec3 asphalt(0.25f, 0.25f, 0.25f);
    glm::vec3 stripeColor(0.95f, 0.85f, 0.10f);

    glm::mat4 mainRoad = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.05f, 0.0f));
    mainRoad = glm::scale(mainRoad, glm::vec3(3.5f, 0.02f, 60.0f));
    drawCube(shaderProgram, vao, mainRoad, asphalt);

    for (float z = -28.0f; z <= 28.0f; z += 4.0f) {
        glm::mat4 stripe = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.07f, z));
        stripe = glm::scale(stripe, glm::vec3(0.2f, 0.02f, 2.0f));
        drawCube(shaderProgram, vao, stripe, stripeColor);
    }

    glm::mat4 roadHall2 = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.05f, -18.0f));
    roadHall2 = glm::scale(roadHall2, glm::vec3(20.0f, 0.02f, 2.5f));
    drawCube(shaderProgram, vao, roadHall2, asphalt);

    glm::mat4 roadHall1_3 = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.05f, 6.0f));
    roadHall1_3 = glm::scale(roadHall1_3, glm::vec3(20.0f, 0.02f, 2.5f));
    drawCube(shaderProgram, vao, roadHall1_3, asphalt);

    glm::mat4 roadHall4 = glm::translate(glm::mat4(1.0f), glm::vec3(-9.0f, 0.05f, 21.0f));
    roadHall4 = glm::scale(roadHall4, glm::vec3(18.0f, 0.02f, 2.5f));
    drawCube(shaderProgram, vao, roadHall4, asphalt);

    glm::mat4 connectRoad = glm::translate(glm::mat4(1.0f), glm::vec3(-12.0f, 0.05f, 13.5f));
    connectRoad = glm::scale(connectRoad, glm::vec3(2.5f, 0.02f, 17.5f));
    drawCube(shaderProgram, vao, connectRoad, asphalt);
}

void drawRoundabout(Shader& shaderProgram, VAO& vao) {
    glm::vec3 asphalt(0.25f, 0.25f, 0.25f);
    glm::vec3 grass(0.2f, 0.6f, 0.2f);
    glm::vec3 monumentColor(0.8f, 0.8f, 0.8f);

    float centerX = 12.0f, centerZ = 27.0f;

    glm::mat4 ring = glm::translate(glm::mat4(1.0f), glm::vec3(centerX, 0.05f, centerZ));
    ring = glm::scale(ring, glm::vec3(8.0f, 0.02f, 8.0f));
    drawCube(shaderProgram, vao, ring, asphalt);

    glm::mat4 island = glm::translate(glm::mat4(1.0f), glm::vec3(centerX, 0.15f, centerZ));
    island = glm::scale(island, glm::vec3(4.0f, 0.2f, 4.0f));
    drawCube(shaderProgram, vao, island, grass);

    glm::mat4 monument = glm::translate(glm::mat4(1.0f), glm::vec3(centerX, 1.2f, centerZ));
    monument = glm::scale(monument, glm::vec3(1.0f, 2.0f, 1.0f));
    drawCube(shaderProgram, vao, monument, monumentColor);

    glm::mat4 legN = glm::translate(glm::mat4(1.0f), glm::vec3(centerX, 0.05f, 22.0f));
    legN = glm::scale(legN, glm::vec3(2.5f, 0.02f, 4.0f));
    drawCube(shaderProgram, vao, legN, asphalt);

    glm::mat4 legW = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.05f, centerZ));
    legW = glm::scale(legW, glm::vec3(8.0f, 0.02f, 2.5f));
    drawCube(shaderProgram, vao, legW, asphalt);
}

void drawTree(Shader& shaderProgram, VAO& vao, float x, float z) {
    glm::vec3 trunkColor(0.35f, 0.20f, 0.10f);
    glm::vec3 foliageColor(0.12f, 0.50f, 0.18f);

    glm::mat4 trunk = glm::translate(glm::mat4(1.0f), glm::vec3(x, 1.25f, z));
    trunk = glm::scale(trunk, glm::vec3(0.4f, 2.5f, 0.4f));
    drawCube(shaderProgram, vao, trunk, trunkColor);

    glm::mat4 f1 = glm::translate(glm::mat4(1.0f), glm::vec3(x, 3.0f, z));
    f1 = glm::scale(f1, glm::vec3(2.0f, 1.5f, 2.0f));
    drawCube(shaderProgram, vao, f1, foliageColor);

    glm::mat4 f2 = glm::translate(glm::mat4(1.0f), glm::vec3(x, 4.2f, z));
    f2 = glm::scale(f2, glm::vec3(1.3f, 1.2f, 1.3f));
    drawCube(shaderProgram, vao, f2, foliageColor);
}

void drawCar(Shader& shaderProgram, VAO& vao, float x, float z, float dirZ) {
    glm::vec3 bodyColor(0.85f, 0.10f, 0.10f);
    glm::vec3 cabinColor(0.15f, 0.20f, 0.30f);
    glm::vec3 wheelColor(0.08f, 0.08f, 0.08f);
    glm::vec3 lightColor(0.95f, 0.95f, 0.40f);

    float rotation = (dirZ >= 0.0f) ? 0.0f : 180.0f;

    glm::mat4 carMat = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
    carMat = glm::rotate(carMat, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 body = glm::translate(carMat, glm::vec3(0.0f, 0.5f, 0.0f));
    body = glm::scale(body, glm::vec3(1.4f, 0.6f, 2.8f));
    drawCube(shaderProgram, vao, body, bodyColor);

    glm::mat4 cabin = glm::translate(carMat, glm::vec3(0.0f, 1.0f, -0.2f));
    cabin = glm::scale(cabin, glm::vec3(1.2f, 0.5f, 1.5f));
    drawCube(shaderProgram, vao, cabin, cabinColor);

    glm::mat4 lightL = glm::translate(carMat, glm::vec3(-0.45f, 0.55f, 1.41f));
    lightL = glm::scale(lightL, glm::vec3(0.25f, 0.15f, 0.05f));
    drawCube(shaderProgram, vao, lightL, lightColor);

    glm::mat4 lightR = glm::translate(carMat, glm::vec3(0.45f, 0.55f, 1.41f));
    lightR = glm::scale(lightR, glm::vec3(0.25f, 0.15f, 0.05f));
    drawCube(shaderProgram, vao, lightR, lightColor);

    float wX[] = { -0.75f, 0.75f, -0.75f, 0.75f };
    float wZ[] = { 0.80f, 0.80f, -0.80f, -0.80f };
    for (int i = 0; i < 4; i++) {
        glm::mat4 wheel = glm::translate(carMat, glm::vec3(wX[i], 0.25f, wZ[i]));
        wheel = glm::scale(wheel, glm::vec3(0.3f, 0.5f, 0.5f));
        drawCube(shaderProgram, vao, wheel, wheelColor);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "KUET Campus Map 3D Scene", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    Shader shaderProgram("default.vert", "default.frag");

    VAO VAO1;
    VAO1.Bind();

    VBO VBO1(vertices, sizeof(vertices));

    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    VAO1.Unbind();
    VBO1.Unbind();

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.45f, 0.70f, 0.90f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Activate();

        int displayW, displayH;
        glfwGetFramebufferSize(window, &displayW, &displayH);
        if (displayH == 0) displayH = 1;
        float aspectRatio = (float)displayW / (float)displayH;

        glm::vec3 cameraPos;
        cameraPos.x = cameraRadius * std::cos(glm::radians(pitch)) * std::cos(glm::radians(yaw));
        cameraPos.y = cameraRadius * std::sin(glm::radians(pitch));
        cameraPos.z = cameraRadius * std::cos(glm::radians(pitch)) * std::sin(glm::radians(yaw));

        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 150.0f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), 20.0f, 40.0f, 20.0f);

        // Ground Plane
        glm::mat4 ground = glm::scale(glm::mat4(1.0f), glm::vec3(70.0f, 0.1f, 70.0f));
        drawCube(shaderProgram, VAO1, ground, glm::vec3(0.25f, 0.55f, 0.22f));

        // Academic Blocks
        drawFourConnectedBlocks(shaderProgram, VAO1, 12.0f);

        // Halls
        drawCourtyardBuilding(shaderProgram, VAO1, -22.0f, -18.0f, 12.0f, 12.0f, 4.5f, 1.8f, glm::vec3(0.85f, 0.75f, 0.65f)); // Hall 2
        drawCourtyardBuilding(shaderProgram, VAO1, -22.0f, 12.0f, 12.0f, 20.0f, 4.5f, 1.8f, glm::vec3(0.85f, 0.75f, 0.65f));  // Hall 3
        drawCourtyardBuilding(shaderProgram, VAO1, -8.0f, 2.0f, 7.0f, 6.0f, 3.5f, 1.2f, glm::vec3(0.80f, 0.70f, 0.60f));      // Hall 1
        drawCourtyardBuilding(shaderProgram, VAO1, -8.0f, 16.0f, 7.0f, 6.0f, 3.5f, 1.2f, glm::vec3(0.80f, 0.70f, 0.60f));     // Hall 4

        // Roads & Roundabout
        drawRoads(shaderProgram, VAO1);
        drawRoundabout(shaderProgram, VAO1);

        // Trees
        float treeZ[] = { -22.0f, -14.0f, -6.0f, 2.0f, 10.0f, 18.0f };
        for (float z : treeZ) {
            drawTree(shaderProgram, VAO1, -3.2f, z);
            drawTree(shaderProgram, VAO1, 3.2f, z);
        }
        drawTree(shaderProgram, VAO1, 16.0f, 22.0f);
        drawTree(shaderProgram, VAO1, 16.0f, 31.0f);

        // Moving Car Animation
        float time = (float)glfwGetTime();
        float carSpeed = 8.0f;
        float roadLength = 50.0f;

        float cycle = fmod(time * carSpeed, roadLength * 2.0f);
        float carZ, dirZ;
        if (cycle < roadLength) {
            carZ = -25.0f + cycle;
            dirZ = 1.0f;
        }
        else {
            carZ = 25.0f - (cycle - roadLength);
            dirZ = -1.0f;
        }

        drawCar(shaderProgram, VAO1, (dirZ > 0 ? 0.8f : -0.8f), carZ, dirZ);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    VAO1.Delete();
    VBO1.Delete();
    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}