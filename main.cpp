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



// Generates a smooth, closed 24-segment 3D cylinder for wheels
void drawCylinder(Shader& shaderProgram, float radius, float width, glm::mat4 model, glm::vec3 color) {
    static GLuint cylVAO = 0;
    static GLsizei vertexCount = 0;

    if (cylVAO == 0) {
        const int SEGMENTS = 24;
        std::vector<GLfloat> vertices;

        for (int i = 0; i < SEGMENTS; i++) {
            float theta1 = (2.0f * glm::pi<float>() * i) / SEGMENTS;
            float theta2 = (2.0f * glm::pi<float>() * (i + 1)) / SEGMENTS;

            float y1 = std::cos(theta1), z1 = std::sin(theta1);
            float y2 = std::cos(theta2), z2 = std::sin(theta2);

            // 1. Outer Curved Wall (Normals outward)
            vertices.insert(vertices.end(), {
                -0.5f, y1, z1,   0.0f, y1, z1,
                 0.5f, y1, z1,   0.0f, y1, z1,
                -0.5f, y2, z2,   0.0f, y2, z2,

                 0.5f, y1, z1,   0.0f, y1, z1,
                 0.5f, y2, z2,   0.0f, y2, z2,
                -0.5f, y2, z2,   0.0f, y2, z2
                });

            // 2. Right Cap (+X Face)
            vertices.insert(vertices.end(), {
                 0.5f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
                 0.5f, y1,   z1,    1.0f, 0.0f, 0.0f,
                 0.5f, y2,   z2,    1.0f, 0.0f, 0.0f
                });

            // 3. Left Cap (-X Face)
            vertices.insert(vertices.end(), {
                -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
                -0.5f, y2,   z2,   -1.0f, 0.0f, 0.0f,
                -0.5f, y1,   z1,   -1.0f, 0.0f, 0.0f
                });
        }

        vertexCount = static_cast<GLsizei>(vertices.size() / 6);

        GLuint cylVBO;
        glGenVertexArrays(1, &cylVAO);
        glGenBuffers(1, &cylVBO);

        glBindVertexArray(cylVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cylVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glm::mat4 scaledModel = glm::scale(model, glm::vec3(width, radius * 2.0f, radius * 2.0f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(scaledModel));
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "objectColor"), color.x, color.y, color.z);

    glBindVertexArray(cylVAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

// Renders a round tire with a metallic rim and visible rotating cross-spokes
void draw3DWheel(Shader& shaderProgram, VAO& vao, glm::mat4 wheelMat, float radius, float width) {
    glm::vec3 tireColor(0.10f, 0.10f, 0.10f);
    glm::vec3 rimColor(0.75f, 0.75f, 0.78f);
    glm::vec3 spokeColor(0.20f, 0.20f, 0.22f);

    // Outer Rubber Tire
    drawCylinder(shaderProgram, radius, width, wheelMat, tireColor);

    // Metallic Rim
    drawCylinder(shaderProgram, radius * 0.65f, width * 1.02f, wheelMat, rimColor);

    // Visible Rotating Spokes (Rotates with wheelMat to show wheel motion)
    glm::mat4 spoke1 = glm::scale(wheelMat, glm::vec3(width * 1.05f, radius * 1.1f, radius * 0.18f));
    drawCube(shaderProgram, vao, spoke1, spokeColor);

    glm::mat4 spoke2 = glm::scale(wheelMat, glm::vec3(width * 1.05f, radius * 0.18f, radius * 1.1f));
    drawCube(shaderProgram, vao, spoke2, spokeColor);
}





void drawBuildingExtension(Shader& shaderProgram, VAO& vao, float xOffset, float zOffset) {
    glm::vec3 pillarColor(0.90f, 0.90f, 0.88f);
    glm::vec3 balconyColor(0.75f, 0.65f, 0.55f);
    // Building boundaries along Z: Front face is at (zOffset + 2.0f)
    float frontFaceZ = zOffset + 2.0f;

    // 2. Front Extended Overhang / Portico Roof (Lower 2-floor height overhang)
    float overhangDepth = 1.5f;
    float overhangZ = frontFaceZ + (overhangDepth / 2.0f); // Center Z of the extended slab
    float canopyHeight = 3.6f;                             // Height of the grand front canopy

    glm::mat4 canopyModel = glm::mat4(1.0f);
    canopyModel = glm::translate(canopyModel, glm::vec3(xOffset, canopyHeight, overhangZ));
    canopyModel = glm::scale(canopyModel, glm::vec3(8.6f, 0.25f, overhangDepth));
    drawCube(shaderProgram, vao, canopyModel, balconyColor);

    // 3. Front Structural Pillars (Colonnade across the front facade)
    const int NUM_PILLARS = 10;
    float pillarZ = frontFaceZ + overhangDepth - 0.15f; // Placed at the outer edge of overhang
    float pillarWidth = 0.2f;
    float pillarHeight = canopyHeight;                  // Extends from ground (Y=0) to canopy

    for (int p = 0; p < NUM_PILLARS; p++) {
        // Evenly space pillars across the building width [-3.9 to +3.9]
        float pillarX = xOffset - 3.9f + (7.8f / (NUM_PILLARS - 1)) * p;

        glm::mat4 pillarModel = glm::mat4(1.0f);
        pillarModel = glm::translate(pillarModel, glm::vec3(pillarX, pillarHeight / 2.0f, pillarZ));
        pillarModel = glm::scale(pillarModel, glm::vec3(pillarWidth, pillarHeight, pillarWidth));
        drawCube(shaderProgram, vao, pillarModel, pillarColor);
    }

    // 4. Horizontal Balcony Bands across the Front Facade (All 5 floors)
    for (int i = 1; i <= 5; i++) {
        float yPos = i * 1.3f;

        // Front horizontal balcony ledge running along the facade
        glm::mat4 frontBalcony = glm::mat4(1.0f);
        frontBalcony = glm::translate(frontBalcony, glm::vec3(xOffset, yPos, frontFaceZ + 0.15f));
        frontBalcony = glm::scale(frontBalcony, glm::vec3(8.3f, 0.18f, 0.30f));
        drawCube(shaderProgram, vao, frontBalcony, balconyColor);
    }
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


        // Side horizontal balcony ledge running along the facade
        glm::mat4 sideBalcony = glm::mat4(1.0f);
        sideBalcony = glm::translate(sideBalcony, glm::vec3(xOffset, yPos, zOffset));
        sideBalcony = glm::scale(sideBalcony, glm::vec3(8.0f, 0.18f, 4.1f));
        drawCube(shaderProgram, vao, sideBalcony, balconyColor);



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

    drawBuildingExtension(shaderProgram, vao, xOffset, zPositions[3]);
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

void drawCar(Shader& shaderProgram, VAO& vao, float x, float z, float dirZ, float wheelRot) {
    // Color Palette (matching reference image)
    glm::vec3 bodyColor(0.28f, 0.29f, 0.31f);      // Dark charcoal gray
    glm::vec3 darkAccent(0.18f, 0.18f, 0.20f);     // Dark trim / bumper color
    glm::vec3 glassColor(0.22f, 0.55f, 0.80f);     // Tinted cyan window glass
    glm::vec3 tailLightColor(0.85f, 0.08f, 0.08f); // Red tail lights
    glm::vec3 headLightColor(0.95f, 0.95f, 0.80f); // Bright headlights

    float rotation = (dirZ >= 0.0f) ? 0.0f : 180.0f;

    glm::mat4 carMat = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
    carMat = glm::rotate(carMat, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

    // -------------------------------------------------------------
    // 1. MAIN LOWER BODY & CHASSIS (Hood, Doors & Trunk Base)
    // -------------------------------------------------------------
    // Main lower body shell
    glm::mat4 body = glm::translate(carMat, glm::vec3(0.0f, 0.42f, 0.0f));
    body = glm::scale(body, glm::vec3(1.50f, 0.45f, 3.40f));
    drawCube(shaderProgram, vao, body, bodyColor);

    // Front Bumper / Grill Guard
    glm::mat4 frontBumper = glm::translate(carMat, glm::vec3(0.0f, 0.30f, 1.71f));
    frontBumper = glm::scale(frontBumper, glm::vec3(1.52f, 0.22f, 0.08f));
    drawCube(shaderProgram, vao, frontBumper, darkAccent);

    // Rear Bumper
    glm::mat4 rearBumper = glm::translate(carMat, glm::vec3(0.0f, 0.30f, -1.71f));
    rearBumper = glm::scale(rearBumper, glm::vec3(1.52f, 0.22f, 0.08f));
    drawCube(shaderProgram, vao, rearBumper, darkAccent);

    // -------------------------------------------------------------
    // 2. CABIN ROOF & PILLARS
    // -------------------------------------------------------------
    // Flat Roof Panel
    glm::mat4 roof = glm::translate(carMat, glm::vec3(0.0f, 1.12f, -0.15f));
    roof = glm::scale(roof, glm::vec3(1.36f, 0.05f, 1.35f));
    drawCube(shaderProgram, vao, roof, bodyColor);

    // Cabin Core Support Frame
    glm::mat4 cabinCore = glm::translate(carMat, glm::vec3(0.0f, 0.88f, -0.15f));
    cabinCore = glm::scale(cabinCore, glm::vec3(1.34f, 0.48f, 1.35f));
    drawCube(shaderProgram, vao, cabinCore, bodyColor);

    // -------------------------------------------------------------
    // 3. SLOPED WINDSHIELDS (FRONT & REAR)
    // -------------------------------------------------------------
    // Front Windshield (Sloped back ~28 degrees)
    glm::mat4 frontGlass = glm::translate(carMat, glm::vec3(0.0f, 0.90f, 0.72f));
    frontGlass = glm::rotate(frontGlass, glm::radians(-28.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    frontGlass = glm::scale(frontGlass, glm::vec3(1.32f, 0.52f, 0.04f));
    drawCube(shaderProgram, vao, frontGlass, glassColor);

    // Rear Windshield (Sloped forward ~26 degrees)
    glm::mat4 rearGlass = glm::translate(carMat, glm::vec3(0.0f, 0.90f, -1.02f));
    rearGlass = glm::rotate(rearGlass, glm::radians(26.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rearGlass = glm::scale(rearGlass, glm::vec3(1.32f, 0.50f, 0.04f));
    drawCube(shaderProgram, vao, rearGlass, glassColor);

    // -------------------------------------------------------------
    // 4. SIDE WINDOW PANES & B-PILLARS
    // -------------------------------------------------------------
    // Left Side Windows
    glm::mat4 winLeftFront = glm::translate(carMat, glm::vec3(-0.685f, 0.90f, 0.18f));
    winLeftFront = glm::scale(winLeftFront, glm::vec3(0.03f, 0.38f, 0.55f));
    drawCube(shaderProgram, vao, winLeftFront, glassColor);

    glm::mat4 winLeftRear = glm::translate(carMat, glm::vec3(-0.685f, 0.90f, -0.48f));
    winLeftRear = glm::scale(winLeftRear, glm::vec3(0.03f, 0.38f, 0.55f));
    drawCube(shaderProgram, vao, winLeftRear, glassColor);

    // Right Side Windows
    glm::mat4 winRightFront = glm::translate(carMat, glm::vec3(0.685f, 0.90f, 0.18f));
    winRightFront = glm::scale(winRightFront, glm::vec3(0.03f, 0.38f, 0.55f));
    drawCube(shaderProgram, vao, winRightFront, glassColor);

    glm::mat4 winRightRear = glm::translate(carMat, glm::vec3(0.685f, 0.90f, -0.48f));
    winRightRear = glm::scale(winRightRear, glm::vec3(0.03f, 0.38f, 0.55f));
    drawCube(shaderProgram, vao, winRightRear, glassColor);

    // Central B-Pillars (Window separator)
    glm::mat4 pillarL = glm::translate(carMat, glm::vec3(-0.69f, 0.90f, -0.15f));
    pillarL = glm::scale(pillarL, glm::vec3(0.04f, 0.40f, 0.08f));
    drawCube(shaderProgram, vao, pillarL, bodyColor);

    glm::mat4 pillarR = glm::translate(carMat, glm::vec3(0.69f, 0.90f, -0.15f));
    pillarR = glm::scale(pillarR, glm::vec3(0.04f, 0.40f, 0.08f));
    drawCube(shaderProgram, vao, pillarR, bodyColor);

    // -------------------------------------------------------------
    // 5. LIGHTING DETAILS (Headlights & Corner Tail Lights)
    // -------------------------------------------------------------
    // Front Headlights
    glm::mat4 headL = glm::translate(carMat, glm::vec3(-0.52f, 0.48f, 1.71f));
    headL = glm::scale(headL, glm::vec3(0.28f, 0.16f, 0.04f));
    drawCube(shaderProgram, vao, headL, headLightColor);

    glm::mat4 headR = glm::translate(carMat, glm::vec3(0.52f, 0.48f, 1.71f));
    headR = glm::scale(headR, glm::vec3(0.28f, 0.16f, 0.04f));
    drawCube(shaderProgram, vao, headR, headLightColor);

    // Vertical Corner Rear Tail Lights (Matching image)
    glm::mat4 tailL = glm::translate(carMat, glm::vec3(-0.66f, 0.50f, -1.71f));
    tailL = glm::scale(tailL, glm::vec3(0.12f, 0.28f, 0.04f));
    drawCube(shaderProgram, vao, tailL, tailLightColor);

    glm::mat4 tailR = glm::translate(carMat, glm::vec3(0.66f, 0.50f, -1.71f));
    tailR = glm::scale(tailR, glm::vec3(0.12f, 0.28f, 0.04f));
    drawCube(shaderProgram, vao, tailR, tailLightColor);

    // -------------------------------------------------------------
    // 6. ROUND ROTATING WHEELS
    // -------------------------------------------------------------
    float wX[] = { -0.78f, 0.78f, -0.78f, 0.78f };
    float wZ[] = { 1.05f, 1.05f, -1.05f, -1.05f };
    float wheelRadius = 0.15f;
    float wheelWidth = 0.22f;

    for (int i = 0; i < 4; i++) {
        glm::mat4 wheelMat = carMat;
        wheelMat = glm::translate(wheelMat, glm::vec3(wX[i], wheelRadius+0.2f, wZ[i]));
        // Rotate around local X axis based on movement distance
        wheelMat = glm::rotate(wheelMat, glm::radians(wheelRot), glm::vec3(1.0f, 0.0f, 0.0f));

        draw3DWheel(shaderProgram, vao, wheelMat, wheelRadius, wheelWidth);
    }
}

void drawBus(Shader& shaderProgram, VAO& vao, float x, float z, float dirZ, float wheelRot){
    // KUET Bus Color Palette
    glm::vec3 whiteBody(0.92f, 0.92f, 0.92f);
    glm::vec3 maroonSkirt(0.50f, 0.08f, 0.12f);
    glm::vec3 maroonStripe(0.60f, 0.10f, 0.15f);
    glm::vec3 windowGlass(0.12f, 0.15f, 0.22f);
    glm::vec3 grilleColor(0.75f, 0.75f, 0.78f);
    glm::vec3 headlightColor(0.98f, 0.95f, 0.65f);
    glm::vec3 wheelTireColor(0.10f, 0.10f, 0.10f);
    glm::vec3 wheelRimColor(0.65f, 0.65f, 0.68f);
    glm::vec3 mirrorColor(0.15f, 0.15f, 0.15f);

    float rotation = (dirZ >= 0.0f) ? 0.0f : 180.0f;

    glm::mat4 busMat = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
    busMat = glm::rotate(busMat, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

    // -------------------------------------------------------------
    // 1. LOWER CHASSIS / MAROON SKIRT (Bottom section)
    // -------------------------------------------------------------
    glm::mat4 skirt = glm::translate(busMat, glm::vec3(0.0f, 0.55f, 0.0f));
    skirt = glm::scale(skirt, glm::vec3(2.2f, 0.7f, 7.6f));
    drawCube(shaderProgram, vao, skirt, maroonSkirt);

    // -------------------------------------------------------------
    // 2. SIGNATURE HORIZONTAL MAROON ACCENT STRIPE
    // -------------------------------------------------------------
    glm::mat4 stripe = glm::translate(busMat, glm::vec3(0.0f, 0.95f, 0.0f));
    stripe = glm::scale(stripe, glm::vec3(2.22f, 0.15f, 7.62f));
    drawCube(shaderProgram, vao, stripe, maroonStripe);

    // -------------------------------------------------------------
    // 3. UPPER MAIN BODY (White cabin core)
    // -------------------------------------------------------------
    glm::mat4 upperBody = glm::translate(busMat, glm::vec3(0.0f, 1.85f, 0.0f));
    upperBody = glm::scale(upperBody, glm::vec3(2.18f, 1.65f, 7.58f));
    drawCube(shaderProgram, vao, upperBody, whiteBody);

    // -------------------------------------------------------------
    // 4. ROOF CAP SLAB
    // -------------------------------------------------------------
    glm::mat4 roof = glm::translate(busMat, glm::vec3(0.0f, 2.72f, 0.0f));
    roof = glm::scale(roof, glm::vec3(2.22f, 0.12f, 7.62f));
    drawCube(shaderProgram, vao, roof, whiteBody);

    // -------------------------------------------------------------
    // 5. SIDE WINDOW PANES (Segmented rows on Left & Right sides)
    // -------------------------------------------------------------
    const int NUM_WINDOWS = 6;
    float windowStartZ = -2.7f;
    float windowSpacingZ = 1.05f;

    for (int i = 0; i < NUM_WINDOWS; i++) {
        float winZ = windowStartZ + i * windowSpacingZ;

        // Left Side Window
        glm::mat4 winL = glm::translate(busMat, glm::vec3(-1.10f, 1.95f, winZ));
        winL = glm::scale(winL, glm::vec3(0.05f, 0.85f, 0.85f));
        drawCube(shaderProgram, vao, winL, windowGlass);

        // Right Side Window
        glm::mat4 winR = glm::translate(busMat, glm::vec3(1.10f, 1.95f, winZ));
        winR = glm::scale(winR, glm::vec3(0.05f, 0.85f, 0.85f));
        drawCube(shaderProgram, vao, winR, windowGlass);
    }

    // -------------------------------------------------------------
    // 6. FRONT WINDSHIELD & REAR WINDOW
    // -------------------------------------------------------------
    // Front Windshield
    glm::mat4 frontGlass = glm::translate(busMat, glm::vec3(0.0f, 2.05f, 3.80f));
    frontGlass = glm::scale(frontGlass, glm::vec3(2.0f, 1.1f, 0.05f));
    drawCube(shaderProgram, vao, frontGlass, windowGlass);

    // Rear Window
    glm::mat4 rearGlass = glm::translate(busMat, glm::vec3(0.0f, 2.05f, -3.80f));
    rearGlass = glm::scale(rearGlass, glm::vec3(1.9f, 0.9f, 0.05f));
    drawCube(shaderProgram, vao, rearGlass, windowGlass);

    // -------------------------------------------------------------
    // 7. FRONT FACADE DETAILS (Grille, Bumper, Lights)
    // -------------------------------------------------------------
    // Front Bumper (Maroon lower shield)
    glm::mat4 frontBumper = glm::translate(busMat, glm::vec3(0.0f, 0.45f, 3.82f));
    frontBumper = glm::scale(frontBumper, glm::vec3(2.2f, 0.5f, 0.08f));
    drawCube(shaderProgram, vao, frontBumper, maroonSkirt);

    // Front Chrome/Silver Grille
    glm::mat4 grille = glm::translate(busMat, glm::vec3(0.0f, 0.85f, 3.83f));
    grille = glm::scale(grille, glm::vec3(1.1f, 0.30f, 0.05f));
    drawCube(shaderProgram, vao, grille, grilleColor);

    // Headlights (Left & Right)
    glm::mat4 headL = glm::translate(busMat, glm::vec3(-0.80f, 0.82f, 3.84f));
    headL = glm::scale(headL, glm::vec3(0.30f, 0.20f, 0.05f));
    drawCube(shaderProgram, vao, headL, headlightColor);

    glm::mat4 headR = glm::translate(busMat, glm::vec3(0.80f, 0.82f, 3.84f));
    headR = glm::scale(headR, glm::vec3(0.30f, 0.20f, 0.05f));
    drawCube(shaderProgram, vao, headR, headlightColor);

    // -------------------------------------------------------------
    // 8. SIDE MIRRORS (Extending from front A-pillars)
    // -------------------------------------------------------------
    // Left Mirror
    glm::mat4 mirrorArmL = glm::translate(busMat, glm::vec3(-1.25f, 2.05f, 3.5f));
    mirrorArmL = glm::scale(mirrorArmL, glm::vec3(0.25f, 0.06f, 0.06f));
    drawCube(shaderProgram, vao, mirrorArmL, mirrorColor);

    glm::mat4 mirrorHeadL = glm::translate(busMat, glm::vec3(-1.38f, 1.95f, 3.5f));
    mirrorHeadL = glm::scale(mirrorHeadL, glm::vec3(0.08f, 0.35f, 0.18f));
    drawCube(shaderProgram, vao, mirrorHeadL, mirrorColor);

    // Right Mirror
    glm::mat4 mirrorArmR = glm::translate(busMat, glm::vec3(1.25f, 2.05f, 3.5f));
    mirrorArmR = glm::scale(mirrorArmR, glm::vec3(0.25f, 0.06f, 0.06f));
    drawCube(shaderProgram, vao, mirrorArmR, mirrorColor);

    glm::mat4 mirrorHeadR = glm::translate(busMat, glm::vec3(1.38f, 1.95f, 3.5f));
    mirrorHeadR = glm::scale(mirrorHeadR, glm::vec3(0.08f, 0.35f, 0.18f));
    drawCube(shaderProgram, vao, mirrorHeadR, mirrorColor);

    // -------------------------------------------------------------
     // 9. ROUND ROTATING WHEELS
     // -------------------------------------------------------------
    float wX[] = { -1.08f, 1.08f, -1.08f, 1.08f };
    float wZ[] = { 2.2f, 2.2f, -2.2f, -2.2f };
    float wheelRadius = 0.3f;
    float wheelWidth = 0.35f;

    for (int i = 0; i < 4; i++) {
        glm::mat4 wheelMat = busMat;
        wheelMat = glm::translate(wheelMat, glm::vec3(wX[i], wheelRadius+0.35f, wZ[i]));
        // Rotate around local X axis based on movement distance
        wheelMat = glm::rotate(wheelMat, glm::radians(wheelRot), glm::vec3(1.0f, 0.0f, 0.0f));

        draw3DWheel(shaderProgram, vao, wheelMat, wheelRadius, wheelWidth);
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
        //float time = (float)glfwGetTime();
        //float carSpeed = 8.0f;
        //float roadLength = 50.0f;

        //float cycle = fmod(time * carSpeed, roadLength * 2.0f);
        //float carZ, dirZ;
        //if (cycle < roadLength) {
        //    carZ = -25.0f + cycle;
        //    dirZ = 1.0f;
        //}
        //else {
        //    carZ = 25.0f - (cycle - roadLength);
        //    dirZ = -1.0f;
        //}

     //   drawCar(shaderProgram, VAO1, (dirZ > 0 ? 0.8f : -0.8f), carZ, dirZ);

        //===========================================================================



        // Vehicle Movement & Rolling Animation Logic
        float time = (float)glfwGetTime();
        float speed = 8.0f;
        float roadLength = 50.0f;

        float cycle = fmod(time * speed, roadLength * 2.0f);
        float vehicleZ, dirZ;
        if (cycle < roadLength) {
            vehicleZ = -25.0f + cycle;
            dirZ = 1.0f;   // Moving forward (+Z)
        }
        else {
            vehicleZ = 25.0f - (cycle - roadLength);
            dirZ = -1.0f;  // Moving backward (-Z)
        }

        // Calculate wheel rotation angles based on distance (θ = distance / radius)
        float carWheelRot = glm::degrees(time * speed / 0.35f);
        float busWheelRot = glm::degrees(time * speed / 0.48f);

        // Draw Car (Left Lane) & Bus (Right Lane) driving together with spinning wheels
        drawCar(shaderProgram, VAO1, (dirZ > 0 ? -1.2f : 1.2f), -vehicleZ, -dirZ, -carWheelRot);
        drawBus(shaderProgram, VAO1, (dirZ > 0 ? 1.4f : -1.4f), vehicleZ, dirZ, busWheelRot);


        //===============================================================================

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