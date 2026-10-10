#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
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
float yaw = 90.0f;          // Horizontal angle (degrees)
float pitch = 20.0f;         // Vertical angle (degrees)
float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool isDragging = false;
bool firstMouse = true;
// Global spotlight state
bool spotlightOn = false;
// Global shading mode tracker (0 = Flat, 1 = Gouraud, 2 = Phong)
int currentShadingMode = 2;
// Global Mode Tracker
bool isDayMode = false;

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
    //if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    //    yaw -= rotateSpeed;
    //if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    //    yaw += rotateSpeed;
    //if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    //    pitch = std::min(pitch + rotateSpeed, 89.0f);
    //if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    //    pitch = std::max(pitch - rotateSpeed, 1.0f);



    // Toggle Spotlight / Street Lights with 'L' Key
    static bool lKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (!lKeyPressed) {
            spotlightOn = !spotlightOn;
            lKeyPressed = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
        lKeyPressed = false;
    }


    //=============================================================

    // Shading Mode Toggles (Q = Flat, W = Gouraud, E = Phong)
    static bool qPressed = false, wPressed = false, ePressed = false;

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if (!qPressed) { currentShadingMode = 0; qPressed = true; }
    }
    else { qPressed = false; }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        if (!wPressed) { currentShadingMode = 1; wPressed = true; }
        // Allows pitch adjustment while holding W without re-triggering mode every frame
        pitch = std::min(pitch + rotateSpeed, 89.0f);
    }
    else { wPressed = false; }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if (!ePressed) { currentShadingMode = 2; ePressed = true; }
    }
    else { ePressed = false; }
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

void drawCorridor(Shader& shaderProgram, VAO& vao, float xOffset, float z1, float z2) {
    glm::vec3 slabColor(0.88f, 0.82f, 0.72f);      // Tan/beige floor slab
    glm::vec3 wallTan(0.78f, 0.68f, 0.58f);        // Parapet railing color
    glm::vec3 pillarColor(0.92f, 0.90f, 0.86f);    // Off-white support columns
    glm::vec3 railingColor(0.75f, 0.75f, 0.78f);   // Metallic silver handrail

    float midZ = (z1 + z2) / 2.0f;
    float gapLength = (z2 - z1) - 3.8f; // Fits tightly inside the 2.0 unit gap between blocks
    float widthX = 2.6f;                // Walkway width in X

    // 1. Structural Vertical Columns on Left and Right Sides of Skybridge
    float leftColumnX = xOffset - (widthX / 2.0f) + 0.15f;
    float rightColumnX = xOffset + (widthX / 2.0f) - 0.15f;

  

    // 2. Multi-Story Open Skybridges (Floors 1 through 4)
    for (int floor = 0; floor <= 4; floor++) {
        float yPos = floor * 1.3f;

        // Walkway Floor Slab
        glm::mat4 floorSlab = glm::mat4(1.0f);
        floorSlab = glm::translate(floorSlab, glm::vec3(xOffset, yPos, midZ));
        floorSlab = glm::scale(floorSlab, glm::vec3(widthX, 0.18f, gapLength));
        drawCube(shaderProgram, vao, floorSlab, slabColor);

        // Front Solid Parapet Wall (+X facing front edge)
        glm::mat4 frontParapet = glm::mat4(1.0f);
        frontParapet = glm::translate(frontParapet, glm::vec3(xOffset + (widthX / 2.0f) - 0.05f, yPos + 0.30f, midZ));
        frontParapet = glm::scale(frontParapet, glm::vec3(0.10f, 0.45f, gapLength));
        drawCube(shaderProgram, vao, frontParapet, wallTan);

        // Back Solid Parapet Wall (-X facing back edge)
        glm::mat4 backParapet = glm::mat4(1.0f);
        backParapet = glm::translate(backParapet, glm::vec3(xOffset - (widthX / 2.0f) + 0.05f, yPos + 0.30f, midZ));
        backParapet = glm::scale(backParapet, glm::vec3(0.10f, 0.45f, gapLength));
        drawCube(shaderProgram, vao, backParapet, wallTan);

        // Top Metallic Safety Handrail Trim
        glm::mat4 frontRail = glm::mat4(1.0f);
        frontRail = glm::translate(frontRail, glm::vec3(xOffset + (widthX / 2.0f) - 0.05f, yPos + 0.55f, midZ));
        frontRail = glm::scale(frontRail, glm::vec3(0.06f, 0.08f, gapLength));
        drawCube(shaderProgram, vao, frontRail, railingColor);

        glm::mat4 backRail = glm::mat4(1.0f);
        backRail = glm::translate(backRail, glm::vec3(xOffset - (widthX / 2.0f) + 0.05f, yPos + 0.55f, midZ));
        backRail = glm::scale(backRail, glm::vec3(0.06f, 0.08f, gapLength));
        drawCube(shaderProgram, vao, backRail, railingColor);
    }

    // Walkway Floor Slab
    glm::mat4 floorSlab = glm::mat4(1.0f);
    floorSlab = glm::translate(floorSlab, glm::vec3(xOffset, 6.5f, midZ));
    floorSlab = glm::scale(floorSlab, glm::vec3(widthX, 0.1f, gapLength));
    drawCube(shaderProgram, vao, floorSlab, slabColor);
}




void drawBuildingBlock(Shader& shaderProgram, VAO& vao, float xOffset, float zOffset) {
    glm::vec3 wallCream(0.94f, 0.92f, 0.88f);     // Light cream upper band
    glm::vec3 wallTan(0.78f, 0.68f, 0.58f);       // Tan/beige lower band
    glm::vec3 darkBase(0.25f, 0.22f, 0.20f);      // Plinth bottom accent
    glm::vec3 windowGlass(0.18f, 0.18f, 0.22f);   // Dark recessed window glass
    glm::vec3 balconyColor(0.82f, 0.74f, 0.64f);

    // 1. Alternating Two-Tone Facade Layering (5 Floors)
    for (int floor = 0; floor < 5; floor++) {
        float floorY = 0.8f + floor * 1.3f;

        // Lower Tan Wall Band
        glm::mat4 tanBand = glm::mat4(1.0f);
        tanBand = glm::translate(tanBand, glm::vec3(xOffset, floorY - 0.25f, zOffset));
        tanBand = glm::scale(tanBand, glm::vec3(8.0f, 0.50f, 4.0f));
        drawCube(shaderProgram, vao, tanBand, wallTan);

        // Upper Cream Wall Band
        glm::mat4 creamBand = glm::mat4(1.0f);
        creamBand = glm::translate(creamBand, glm::vec3(xOffset, floorY + 0.35f, zOffset));
        creamBand = glm::scale(creamBand, glm::vec3(8.0f, 0.70f, 4.0f));
        drawCube(shaderProgram, vao, creamBand, wallCream);

        // Horizontal Ribbon Windows (Front & Back Faces)
        for (int win = -3; win <= 3; win += 2) {
            float winX = xOffset + win * 1.0f;

            // +Z Face Window Strip
            glm::mat4 winFront = glm::mat4(1.0f);
            winFront = glm::translate(winFront, glm::vec3(winX, floorY + 0.35f, zOffset + 2.01f));
            winFront = glm::scale(winFront, glm::vec3(1.4f, 0.35f, 0.04f));
            drawCube(shaderProgram, vao, winFront, windowGlass);

            // -Z Face Window Strip
            glm::mat4 winBack = glm::mat4(1.0f);
            winBack = glm::translate(winBack, glm::vec3(winX, floorY + 0.35f, zOffset - 2.01f));
            winBack = glm::scale(winBack, glm::vec3(1.4f, 0.35f, 0.04f));
            drawCube(shaderProgram, vao, winBack, windowGlass);
        }

        // Side Balcony Ledging
        if (floor > 0) {
            glm::mat4 sideBalcony = glm::mat4(1.0f);
            sideBalcony = glm::translate(sideBalcony, glm::vec3(xOffset, floorY - 0.25f, zOffset));
            sideBalcony = glm::scale(sideBalcony, glm::vec3(8.1f, 0.15f, 4.1f));
            drawCube(shaderProgram, vao, sideBalcony, balconyColor);
        }
    }

    // Dark Bottom Plinth Base
    glm::mat4 base = glm::mat4(1.0f);
    base = glm::translate(base, glm::vec3(xOffset, 0.25f, zOffset));
    base = glm::scale(base, glm::vec3(8.05f, 0.50f, 4.05f));
    drawCube(shaderProgram, vao, base, darkBase);

    // Side Oval Balconies (+X and -X ends)
    float balconyOutwardRadius = 1.5f;
    float balconyHeight = 0.2f;
    float balconyDepthRadius = 1.8f;

    for (int i = 1; i <= 4; i++) {
        float yPos = i * 1.3f;
        glm::vec3 posPlusX(xOffset + 3.0f, yPos+0.5f, zOffset);
        drawSemiOval(shaderProgram, balconyOutwardRadius, balconyHeight, balconyDepthRadius, posPlusX, 0.0f, balconyColor);

        glm::vec3 posMinusX(xOffset - 3.0f, yPos + 0.5f, zOffset);
        drawSemiOval(shaderProgram, balconyOutwardRadius, balconyHeight, balconyDepthRadius, posMinusX, 180.0f, balconyColor);
    }

    glm::vec3 doorColor(0.18f, 0.18f, 0.22f);

    for (int i = 0; i <= 4; i++) {
        float yPos = i * 1.3f + 0.5f;

        // +X side door
        glm::mat4 doorPlus = glm::mat4(1.0f);
        doorPlus = glm::translate(
            doorPlus,
            glm::vec3(xOffset + 4.0f, yPos + 0.55f, zOffset)
        );
        doorPlus = glm::scale(
            doorPlus,
            glm::vec3(0.08f, 0.75f, 0.8f)
        );
        drawCube(shaderProgram, vao, doorPlus, doorColor);

        // -X side door
        glm::mat4 doorMinus = glm::mat4(1.0f);
        doorMinus = glm::translate(
            doorMinus,
            glm::vec3(xOffset - 4.02f, yPos + 0.55f, zOffset)
        );
        doorMinus = glm::scale(
            doorMinus,
            glm::vec3(0.08f, 0.75f, 0.8f)
        );
        drawCube(shaderProgram, vao, doorMinus, doorColor);
    }

    for (int s = 0; s < 3; s++) {
        float stepY = (s * 0.08f) + 0.04f;
        float stepDepth = 0.30f + (3 - s) * 0.20f;
        glm::mat4 stepMat = glm::translate(glm::mat4(1.0f), glm::vec3(xOffset-4.0f, 0.1f, zOffset));
        stepMat = glm::scale(stepMat, glm::vec3(stepDepth, stepY, 2.4f + s * 0.2f));
        drawCube(shaderProgram, vao, stepMat, glm::vec3(0.93f, 0.93f, 0.90f));



        glm::mat4 stepMat2 = glm::translate(glm::mat4(1.0f), glm::vec3(xOffset + 4.0f, 0.1f, zOffset));
        stepMat2 = glm::scale(stepMat2, glm::vec3(stepDepth, stepY, 2.4f + s * 0.2f));
        drawCube(shaderProgram, vao, stepMat2, glm::vec3(0.93f, 0.93f, 0.90f));
    }


    // =============================================================
// FRONT & BACK CENTER DOORS (+Z and -Z sides)
// =============================================================

    glm::vec3 entranceDoorColor(0.16f, 0.18f, 0.20f);

    // Door dimensions
    float doorWidth = 1.2f;
    float doorHeight = 1.5f;

    // -------------------------------------------------------------
    // FRONT DOOR (+Z side)
    // -------------------------------------------------------------
    glm::mat4 frontDoor = glm::mat4(1.0f);

    frontDoor = glm::translate(
        frontDoor,
        glm::vec3(
            xOffset,
            0.95f,
            zOffset + 2.05f
        )
    );

    frontDoor = glm::scale(
        frontDoor,
        glm::vec3(
            doorWidth,
            doorHeight,
            0.08f
        )
    );

    drawCube(
        shaderProgram,
        vao,
        frontDoor,
        entranceDoorColor
    );


    // -------------------------------------------------------------
    // BACK DOOR (-Z side)
    // -------------------------------------------------------------
    glm::mat4 backDoor = glm::mat4(1.0f);

    backDoor = glm::translate(
        backDoor,
        glm::vec3(
            xOffset,
            0.95f,
            zOffset - 2.05f
        )
    );

    backDoor = glm::scale(
        backDoor,
        glm::vec3(
            doorWidth,
            doorHeight,
            0.08f
        )
    );

    drawCube(
        shaderProgram,
        vao,
        backDoor,
        entranceDoorColor
    );


    // =============================================================
// FRONT & BACK ENTRANCE STAIRS
// =============================================================

    glm::vec3 stairColor(0.88f, 0.88f, 0.84f);

    int numSteps = 4;

    float stepHeight = 0.12f;
    float stepWidth = 1.8f;
    float stepDepth = 0.45f;

    // =============================================================
    // FRONT STAIRS (+Z)
    // =============================================================

    for (int s = 0; s < numSteps; s++) {

        float y = 0.25f - s * stepHeight;

        // Each higher step moves closer to the building
        float z = zOffset + 2.25f + s * stepDepth;

        glm::mat4 step = glm::mat4(1.0f);

        step = glm::translate(
            step,
            glm::vec3(
                xOffset,
                y,
                -z
            )
        );

        step = glm::scale(
            step,
            glm::vec3(
                stepWidth,
                stepHeight,
                stepDepth
            )
        );

        drawCube(
            shaderProgram,
            vao,
            step,
            stairColor
        );
    }


    // =============================================================
    // BACK STAIRS (-Z)
    // =============================================================

    for (int s = 0; s < numSteps; s++) {

        float y = 0.12f - s * stepHeight;

        // Move steps toward the -Z direction
        float z = zOffset - 2.25f - s * stepDepth;

        glm::mat4 step = glm::mat4(1.0f);

        step = glm::translate(
            step,
            glm::vec3(
                xOffset,
                y,
                -z
            )
        );

        step = glm::scale(
            step,
            glm::vec3(stepWidth,stepHeight,stepDepth)
        );

        drawCube(shaderProgram,vao,step,stairColor);
    }



}

void drawFourConnectedBlocks(Shader& shaderProgram, VAO& vao, float xOffset) {
    float zPositions[] = { -9.0f, -3.0f, 3.0f, 9.0f };
    glm::vec3 corridorColor(0.93f, 0.88f, 0.80f);

    for (int i = 0; i < 4; i++) {
        drawBuildingBlock(shaderProgram, vao, xOffset, zPositions[i]);

        if (i < 3) {
            drawCorridor(shaderProgram, vao, xOffset, zPositions[i], zPositions[i + 1]);
        }
    }

    drawBuildingExtension(shaderProgram, vao, xOffset, zPositions[3]);
}

void drawFFHDRH(Shader& shader, VAO& vao, float x, float z, float w, float d, float h, float wallThick, glm::vec3 color) {
    // Architectural Color Palette
    glm::vec3 pinkWall(0.88f, 0.68f, 0.72f);      // Main facade plaster
    glm::vec3 whiteTrim(0.93f, 0.93f, 0.90f);     // Concrete pillars, ledges & parapets
    glm::vec3 darkBase(0.25f, 0.22f, 0.20f);      // Plinth ground base
    glm::vec3 windowGlass(0.18f, 0.22f, 0.28f);   // Dark recessed window glass
    glm::vec3 doorColor(0.12f, 0.12f, 0.14f);     // Entrance doorway frame

    float h1 = h / 3.0f; // Height of 1-story section

    // Wall Center Coordinates
    float northZ = z - d / 2.0f + wallThick / 2.0f; // North Wall (-Z)
    float southZ = z + d / 2.0f - wallThick / 2.0f; // South Wall (+Z)
    float westX = x - w / 2.0f + wallThick / 2.0f; // West Wall (-X)
    float eastX = x + w / 2.0f - wallThick / 2.0f; // East Wall (+X)

    float sideWingDepth = d - 2.0f * wallThick;

    // =========================================================================
    // 1. SOUTH WING (1-STORY FRONT WALL)
    // =========================================================================
    float entryWidth = wallThick; // Reserve space for SE entry point
    float southWallWidth = w - entryWidth;
    float southWallCenterX = x - entryWidth / 2.0f;

    // 1-Story South Wall Body
    glm::mat4 southWing = glm::translate(glm::mat4(1.0f), glm::vec3(southWallCenterX, h1 / 2.0f, southZ));
    southWing = glm::scale(southWing, glm::vec3(southWallWidth, h1, wallThick));
    drawCube(shader, vao, southWing, pinkWall);

    // South Wing Plinth Base
    glm::mat4 baseS = glm::translate(glm::mat4(1.0f), glm::vec3(southWallCenterX, 0.15f, southZ));
    baseS = glm::scale(baseS, glm::vec3(southWallWidth + 0.05f, 0.30f, wallThick + 0.05f));
    drawCube(shader, vao, baseS, darkBase);

    // South Wing Parapet Cap
    glm::mat4 roofS1 = glm::translate(glm::mat4(1.0f), glm::vec3(southWallCenterX, h1 + 0.10f, southZ));
    roofS1 = glm::scale(roofS1, glm::vec3(southWallWidth, 0.20f, wallThick + 0.08f));
    drawCube(shader, vao, roofS1, whiteTrim);

    // Ground Floor Windows on 1-Story South Wall
    int numSouthWins = 3;
    float stepSouthWin = southWallWidth / (float)(numSouthWins + 1);
    for (int i = 1; i <= numSouthWins; i++) {
        float winX = (southWallCenterX - southWallWidth / 2.0f) + i * stepSouthWin;
        glm::mat4 win = glm::translate(glm::mat4(1.0f), glm::vec3(winX, h1 * 0.55f, southZ + wallThick / 2.0f + 0.02f));
        win = glm::scale(win, glm::vec3(stepSouthWin * 0.45f, h1 * 0.40f, 0.05f));
        drawCube(shader, vao, win, windowGlass);
    }

    // =========================================================================
    // 2. ENTRY POINT (FACING EAST (+X) AT SOUTH-EAST CORNER WITH STAIRS)
    // =========================================================================
    // 2. ENTRY POINT (FACING EAST (+X) AT SOUTH-EAST CORNER WITH STAIRS)
    float entryLengthZ = wallThick ;        // Entry width along Z-axis
    float entryZ = southZ;                        // South-East corner position
    float outerEastX = eastX- wallThick / 2.0f;  // Outer face of East wall

    // Entrance Door Canopy
    glm::mat4 canopy = glm::translate(glm::mat4(1.0f), glm::vec3(outerEastX, h1 * 0.85f, entryZ));
    canopy = glm::scale(canopy, glm::vec3(0.70f, 0.15f, entryLengthZ * 0.90f));
    drawCube(shader, vao, canopy, whiteTrim);

    // Entrance Doorway Opening Frame
    glm::mat4 doorMat = glm::translate(glm::mat4(1.0f), glm::vec3(outerEastX, h1 * 0.40f, entryZ));
    doorMat = glm::scale(doorMat, glm::vec3(0.06f, h1 * 0.70f, entryLengthZ * 0.70f));
    drawCube(shader, vao, doorMat, doorColor);

    // Multi-Step Entrance Stairs
    int numSteps = 5;
    for (int s = 0; s < numSteps; s++) {
        float stepY = (s * 0.06f) + 0.03f;
        float stepDepth = 0.25f + (numSteps - s) * 0.15f;
        glm::mat4 stepMat = glm::translate(glm::mat4(1.0f), glm::vec3(outerEastX + stepDepth / 2.0f, stepY, entryZ));
        stepMat = glm::scale(stepMat, glm::vec3(stepDepth, 0.06f, entryLengthZ * 0.85f));
        drawCube(shader, vao, stepMat, whiteTrim);
    }



    // =========================================================================
    // 3. MAIN WINGS (3-STORY WEST & EAST, 1-STORY NORTH)
    // =========================================================================
    // West Wing (-X) - 3 Stories
    glm::mat4 westWing = glm::translate(glm::mat4(1.0f), glm::vec3(westX, h / 2.0f, z));
    westWing = glm::scale(westWing, glm::vec3(wallThick, h, sideWingDepth + 2.0f * wallThick));
    drawCube(shader, vao, westWing, pinkWall);

    // East Wing (+X) - 3 Stories
    glm::mat4 eastWing = glm::translate(glm::mat4(1.0f), glm::vec3(eastX, h / 2.0f, z - wallThick / 2.0f));
    eastWing = glm::scale(eastWing, glm::vec3(wallThick, h, sideWingDepth + wallThick));
    drawCube(shader, vao, eastWing, pinkWall);

    // North Wing (-Z) - 1 Story
    glm::mat4 northWing = glm::translate(glm::mat4(1.0f), glm::vec3(x-wallThick/2.0f, h1 / 2.0f, northZ-wallThick/2.0f));
    northWing = glm::scale(northWing, glm::vec3(w - wallThick, h1, wallThick+wallThick));
    drawCube(shader, vao, northWing, pinkWall);

    // Plinth Bases
    glm::mat4 baseW = glm::translate(glm::mat4(1.0f), glm::vec3(westX, 0.15f, z));
    baseW = glm::scale(baseW, glm::vec3(wallThick + 0.05f, 0.30f, sideWingDepth + 2.0f * wallThick));
    drawCube(shader, vao, baseW, darkBase);

    glm::mat4 baseE = glm::translate(glm::mat4(1.0f), glm::vec3(eastX, 0.15f, z - wallThick / 2.0f));
    baseE = glm::scale(baseE, glm::vec3(wallThick + 0.05f, 0.30f, sideWingDepth + wallThick));
    drawCube(shader, vao, baseE, darkBase);

    glm::mat4 baseN = glm::translate(glm::mat4(1.0f), glm::vec3(x - wallThick / 2.0f, 0.15f, northZ - wallThick / 2.0f));
    baseN = glm::scale(baseN, glm::vec3(w - wallThick + 0.05f, 0.30f, wallThick + wallThick + 0.05f));
    drawCube(shader, vao, baseN, darkBase);

    // Roof Top Parapet Caps
    glm::mat4 roofW = glm::translate(glm::mat4(1.0f), glm::vec3(westX, h + 0.10f, z));
    roofW = glm::scale(roofW, glm::vec3(wallThick + 0.10f, 0.20f, sideWingDepth + 2.0f * wallThick + 0.10f));
    drawCube(shader, vao, roofW, whiteTrim);

    glm::mat4 roofE = glm::translate(glm::mat4(1.0f), glm::vec3(eastX, h + 0.10f, z - wallThick / 2.0f));
    roofE = glm::scale(roofE, glm::vec3(wallThick + 0.10f, 0.20f, sideWingDepth + wallThick + 0.10f));
    drawCube(shader, vao, roofE, whiteTrim);

    glm::mat4 roofN = glm::translate(glm::mat4(1.0f), glm::vec3(x - wallThick / 2.0f, h1 + 0.10f, northZ - wallThick / 2.0f));
    roofN = glm::scale(roofN, glm::vec3(w - wallThick, 0.20f, wallThick + wallThick + 0.10f));
    drawCube(shader, vao, roofN, whiteTrim);

    // =========================================================================
    // 4. FACADE DETAILS (LEDGES, WINDOWS & 1-STORY NORTH PILLARS)
    // =========================================================================
    // Horizontal Concrete Ledges for 3-Story West & East Wings
    for (int floor = 1; floor <= 3; floor++) {
        float ledgeY = floor * h1;

        // West Outer Ledge
        glm::mat4 lW = glm::translate(glm::mat4(1.0f), glm::vec3(westX - wallThick / 2.0f - 0.02f, ledgeY, z));
        lW = glm::scale(lW, glm::vec3(0.08f, 0.12f, sideWingDepth + 2.0f * wallThick));
        drawCube(shader, vao, lW, whiteTrim);

        // East Outer Ledge
        glm::mat4 lE = glm::translate(glm::mat4(1.0f), glm::vec3(eastX + wallThick / 2.0f + 0.02f, ledgeY, z - wallThick / 2.0f));
        lE = glm::scale(lE, glm::vec3(0.08f, 0.12f, sideWingDepth + wallThick));
        drawCube(shader, vao, lE, whiteTrim);
    }

    // North Inner Ledge at 1-Story Roof Height
    glm::mat4 lN = glm::translate(glm::mat4(1.0f), glm::vec3(x, h1, northZ + wallThick / 2.0f + 0.02f));
    lN = glm::scale(lN, glm::vec3(w - 2.0f * wallThick, 0.12f, 0.08f));
    drawCube(shader, vao, lN, whiteTrim);

    // Stacked Windows on 3-Story West & East South-Facing Front Ends
    for (int f = 0; f < 3; f++) {
        float winY = (f * h1) + (h1 * 0.55f);

        // West Wing South End Window
        glm::mat4 winW = glm::translate(glm::mat4(1.0f), glm::vec3(westX, winY, southZ + wallThick / 2.0f + 0.02f));
        winW = glm::scale(winW, glm::vec3(wallThick * 0.60f, h1 * 0.40f, 0.05f));
        drawCube(shader, vao, winW, windowGlass);

       
    }

    // 1-Story Vertical Structural Accent Pillars on North Courtyard Wall
    float colOffsetsX[2] = { x - w * 0.20f, x + w * 0.20f };
    for (int c = 0; c < 2; c++) {
        glm::mat4 colMat = glm::translate(glm::mat4(1.0f), glm::vec3(colOffsetsX[c], h1 / 2.0f, northZ + wallThick / 2.0f + 0.04f));
        colMat = glm::scale(colMat, glm::vec3(0.25f, h1, 0.08f));
        drawCube(shader, vao, colMat, whiteTrim);
    }

    // Ground Floor Windows on 1-Story North Wall
    float winY_North = h1 * 0.55f;
    glm::mat4 winN1 = glm::translate(glm::mat4(1.0f), glm::vec3(x, winY_North, northZ + wallThick / 2.0f + 0.02f));
    winN1 = glm::scale(winN1, glm::vec3(5.0f, h1 * 0.50f, 0.05f));
    drawCube(shader, vao, winN1, windowGlass);

   


    // =========================================================================
    // EXTERNAL WINDOW GRID (OUTER FACADES)
    // =========================================================================
    int baysZ = 6;
    float stepZ = sideWingDepth / (float)baysZ;

    // --- Outer West Wall (-X) Windows (3 Floors) ---
    float outerWestX = westX - wallThick / 2.0f - 0.02f;
    for (int floor = 0; floor < 3; floor++) {
        float winY = (floor * h1) + (h1 * 0.55f);
        for (int b = 0; b < baysZ; b++) {
            float winZ = (z - sideWingDepth / 2.0f + stepZ / 2.0f) + b * stepZ;
            glm::mat4 winMat = glm::translate(glm::mat4(1.0f), glm::vec3(outerWestX, winY, winZ));
            winMat = glm::scale(winMat, glm::vec3(0.05f, h1 * 0.40f, stepZ * 0.55f));
            drawCube(shader, vao, winMat, windowGlass);
        }
    }

    // --- Outer East Wall (+X) Windows (3 Floors) ---
    float outerEastX_Wall = eastX + wallThick / 2.0f + 0.02f;
    for (int floor = 0; floor < 3; floor++) {
        float winY = (floor * h1) + (h1 * 0.55f);
        for (int b = 0; b < baysZ; b++) {
            float winZ = (z - sideWingDepth / 2.0f + stepZ / 2.0f) + b * stepZ;
            glm::mat4 winMat = glm::translate(glm::mat4(1.0f), glm::vec3(outerEastX_Wall, winY, winZ));
            winMat = glm::scale(winMat, glm::vec3(0.05f, h1 * 0.40f, stepZ * 0.55f));
            drawCube(shader, vao, winMat, windowGlass);
        }
    }

    // --- Outer North Wall (-Z) Windows (Ground Floor) ---
    float outerNorthZ = northZ - wallThick / 2.0f - 0.02f;
    int baysX = 6;
    float stepX = (w - 2.0f * wallThick) / (float)baysX;
    float winY_Ground = h1 * 0.55f;
    for (int b = 0; b < baysX; b++) {
        float winX = (x - (w - 2.0f * wallThick) / 2.0f + stepX / 2.0f) + b * stepX;
        glm::mat4 winMat = glm::translate(glm::mat4(1.0f), glm::vec3(winX, winY_Ground, outerNorthZ-wallThick));
        winMat = glm::scale(winMat, glm::vec3(stepX * 0.55f, h1 * 0.40f, 0.05f));
        drawCube(shader, vao, winMat, windowGlass);
    }



    // =========================================================================
    // INNER COURTYARD PILLARS & WINDOWS (WEST & EAST INNER WALLS)
    // =========================================================================
    float innerWestX = westX + wallThick / 2.0f + 0.02f; // Inner face of West wall
    float innerEastX = eastX - wallThick / 2.0f - 0.02f; // Inner face of East wall

    // Z-positions for pillars flanking the window ends
    float colOffsetsZ[4] = { z - sideWingDepth * 0.40f, z + sideWingDepth * 0.40f, z - sideWingDepth * 0.10f, z + sideWingDepth * 0.10f };

    // -------------------------------------------------------------------------
    // 1. WEST INNER WALL (-X side, facing courtyard)
    // -------------------------------------------------------------------------
    // 3-Story Vertical Structural Pillars
    for (int c = 0; c < 4; c++) {
        glm::mat4 colMat = glm::translate(glm::mat4(1.0f), glm::vec3(innerWestX + 0.02f, h / 2.0f, colOffsetsZ[c]));
        colMat = glm::scale(colMat, glm::vec3(0.08f, h, 0.1f));
        drawCube(shader, vao, colMat, whiteTrim);
    }

    // 3-Floor Windows (Framed between the pillars)
    for (int f = 0; f < 3; f++) {
        float winY = (f * h1) + (h1 * 0.55f);
        glm::mat4 winWestInner = glm::translate(glm::mat4(1.0f), glm::vec3(innerWestX, winY, z));
        winWestInner = glm::scale(winWestInner, glm::vec3(0.05f, h1 * 0.50f, sideWingDepth+1.5f ));
        drawCube(shader, vao, winWestInner, windowGlass);
    }

    // -------------------------------------------------------------------------
    // 2. EAST INNER WALL (+X side, facing courtyard)
    // -------------------------------------------------------------------------
    // 3-Story Vertical Structural Pillars
    for (int c = 0; c < 4; c++) {
        glm::mat4 colMat = glm::translate(glm::mat4(1.0f), glm::vec3(innerEastX - 0.02f, h / 2.0f, colOffsetsZ[c]));
        colMat = glm::scale(colMat, glm::vec3(0.08f, h, 0.1f));
        drawCube(shader, vao, colMat, whiteTrim);
    }

    // 3-Floor Windows (Framed between the pillars)
    for (int f = 0; f < 3; f++) {
        float winY = (f * h1) + (h1 * 0.55f);
        glm::mat4 winEastInner = glm::translate(glm::mat4(1.0f), glm::vec3(innerEastX, winY, z));
        winEastInner = glm::scale(winEastInner, glm::vec3(0.05f, h1 * 0.50f, sideWingDepth ));
        drawCube(shader, vao, winEastInner, windowGlass);
    }

}



void drawSSHAKH(Shader& shader, VAO& vao, float x, float z, float w, float d, float h, float wallThick, glm::vec3 color) {
    // Architectural Color Palette
    glm::vec3 pinkWall(0.88f, 0.68f, 0.72f);      // Main facade plaster
    glm::vec3 whiteTrim(0.93f, 0.93f, 0.90f);     // Concrete pillars, ledges & parapets
    glm::vec3 darkBase(0.25f, 0.22f, 0.20f);      // Plinth ground base
    glm::vec3 windowGlass(0.18f, 0.22f, 0.28f);   // Dark recessed window glass
    glm::vec3 doorColor(0.12f, 0.12f, 0.14f);     // Entrance doorway frame

    float h1 = h / 5.0f; // Height of 1-story section

    // Wall Center Coordinates
    float northZ = z - d / 2.0f + wallThick / 2.0f; // North Wall (-Z)
    float southZ = z + d / 2.0f - wallThick / 2.0f; // South Wall (+Z)
    float westX = x - w / 2.0f + wallThick / 2.0f; // West Wall (-X)
    float eastX = x + w / 2.0f - wallThick / 2.0f; // East Wall (+X)

    float sideWingDepth = d - 2.0f * wallThick;
    float wingWidth = w - 2.0f * wallThick;

    // =========================================================================
    // 1. ALL 4 MAIN WINGS (5 STORIES FULL HEIGHT)
    // =========================================================================
    // West Wing (-X) - 5 Stories
    glm::mat4 westWing = glm::translate(glm::mat4(1.0f), glm::vec3(westX, h / 2.0f, z));
    westWing = glm::scale(westWing, glm::vec3(wallThick, h, d));
    drawCube(shader, vao, westWing, pinkWall);

    // East Wing (+X) - 5 Stories
    glm::mat4 eastWing = glm::translate(glm::mat4(1.0f), glm::vec3(eastX, h / 2.0f, z));
    eastWing = glm::scale(eastWing, glm::vec3(wallThick, h, d));
    drawCube(shader, vao, eastWing, pinkWall);

    // North Wing (-Z) - 5 Stories
    glm::mat4 northWing = glm::translate(glm::mat4(1.0f), glm::vec3(x, h / 2.0f, northZ));
    northWing = glm::scale(northWing, glm::vec3(wingWidth, h, wallThick));
    drawCube(shader, vao, northWing, pinkWall);

    // South Wing (+Z) - 5 Stories
    glm::mat4 southWing = glm::translate(glm::mat4(1.0f), glm::vec3(x, h / 2.0f, southZ));
    southWing = glm::scale(southWing, glm::vec3(wingWidth, h, wallThick));
    drawCube(shader, vao, southWing, pinkWall);

    // -------------------------------------------------------------------------
    // Plinth Bases (All 4 Wings)
    // -------------------------------------------------------------------------
    glm::mat4 baseW = glm::translate(glm::mat4(1.0f), glm::vec3(westX, 0.15f, z));
    baseW = glm::scale(baseW, glm::vec3(wallThick + 0.05f, 0.30f, d + 0.05f));
    drawCube(shader, vao, baseW, darkBase);

    glm::mat4 baseE = glm::translate(glm::mat4(1.0f), glm::vec3(eastX, 0.15f, z));
    baseE = glm::scale(baseE, glm::vec3(wallThick + 0.05f, 0.30f, d + 0.05f));
    drawCube(shader, vao, baseE, darkBase);

    glm::mat4 baseN = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.15f, northZ));
    baseN = glm::scale(baseN, glm::vec3(wingWidth + 0.05f, 0.30f, wallThick + 0.05f));
    drawCube(shader, vao, baseN, darkBase);

    glm::mat4 baseS = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.15f, southZ));
    baseS = glm::scale(baseS, glm::vec3(wingWidth + 0.05f, 0.30f, wallThick + 0.05f));
    drawCube(shader, vao, baseS, darkBase);

    // -------------------------------------------------------------------------
    // Roof Parapet Caps (All 4 Wings at h)
    // -------------------------------------------------------------------------
    glm::mat4 roofW = glm::translate(glm::mat4(1.0f), glm::vec3(westX, h + 0.10f, z));
    roofW = glm::scale(roofW, glm::vec3(wallThick + 0.10f, 0.20f, d + 0.10f));
    drawCube(shader, vao, roofW, whiteTrim);

    glm::mat4 roofE = glm::translate(glm::mat4(1.0f), glm::vec3(eastX, h + 0.10f, z));
    roofE = glm::scale(roofE, glm::vec3(wallThick + 0.10f, 0.20f, d + 0.10f));
    drawCube(shader, vao, roofE, whiteTrim);

    glm::mat4 roofN = glm::translate(glm::mat4(1.0f), glm::vec3(x, h + 0.10f, northZ));
    roofN = glm::scale(roofN, glm::vec3(wingWidth + 0.10f, 0.20f, wallThick + 0.10f));
    drawCube(shader, vao, roofN, whiteTrim);

    glm::mat4 roofS = glm::translate(glm::mat4(1.0f), glm::vec3(x, h + 0.10f, southZ));
    roofS = glm::scale(roofS, glm::vec3(wingWidth + 0.10f, 0.20f, wallThick + 0.10f));
    drawCube(shader, vao, roofS, whiteTrim);

    // =========================================================================
    // 2. MIDDLE ENTRANCE (MIDDLE OUTER PART OF EAST WALL +X)
    // =========================================================================
    float entryLengthZ = wallThick * 2.5f;       // Entrance width along Z
    float outerEastX = eastX + wallThick / 2.0f; // Outer face of East wall

    // Entrance Canopy Portico (Center of East Wall)
    glm::mat4 canopy = glm::translate(glm::mat4(1.0f), glm::vec3(outerEastX + 0.35f, h1 * 0.85f, z));
    canopy = glm::scale(canopy, glm::vec3(0.80f, 0.15f, entryLengthZ));
    drawCube(shader, vao, canopy, whiteTrim);

    // Entrance Doorway Frame
    glm::mat4 doorMat = glm::translate(glm::mat4(1.0f), glm::vec3(outerEastX + 0.01f, h1 * 0.40f, z));
    doorMat = glm::scale(doorMat, glm::vec3(0.06f, h1 * 0.70f, entryLengthZ * 0.70f));
    drawCube(shader, vao, doorMat, doorColor);

    // Multi-Step Entrance Stairs (Facing +X)
    int numSteps = 5;
    for (int s = 0; s < numSteps; s++) {
        float stepY = (s * 0.06f) + 0.03f;
        float stepDepth = 0.25f + (numSteps - s) * 0.15f;
        glm::mat4 stepMat = glm::translate(glm::mat4(1.0f), glm::vec3(outerEastX + stepDepth / 2.0f, stepY, z));
        stepMat = glm::scale(stepMat, glm::vec3(stepDepth, 0.06f, entryLengthZ * 0.90f));
        drawCube(shader, vao, stepMat, whiteTrim);
    }

    // =========================================================================
    // 3. FACADE LEDGES & EXTERNAL WINDOWS (ALL 5 FLOORS)
    // =========================================================================
    // Horizontal Ledges for 5 Floors
    for (int floor = 1; floor <= 5; floor++) {
        float ledgeY = floor * h1;

        // West Outer Ledge
        glm::mat4 lW = glm::translate(glm::mat4(1.0f), glm::vec3(westX - wallThick / 2.0f - 0.02f, ledgeY, z));
        lW = glm::scale(lW, glm::vec3(0.08f, 0.12f, d));
        drawCube(shader, vao, lW, whiteTrim);

        // East Outer Ledge
        glm::mat4 lE = glm::translate(glm::mat4(1.0f), glm::vec3(eastX + wallThick / 2.0f + 0.02f, ledgeY, z));
        lE = glm::scale(lE, glm::vec3(0.08f, 0.12f, d));
        drawCube(shader, vao, lE, whiteTrim);

        // North Outer Ledge
        glm::mat4 lN = glm::translate(glm::mat4(1.0f), glm::vec3(x, ledgeY, northZ - wallThick / 2.0f - 0.02f));
        lN = glm::scale(lN, glm::vec3(w, 0.12f, 0.08f));
        drawCube(shader, vao, lN, whiteTrim);

        // South Outer Ledge
        glm::mat4 lS = glm::translate(glm::mat4(1.0f), glm::vec3(x, ledgeY, southZ + wallThick / 2.0f + 0.02f));
        lS = glm::scale(lS, glm::vec3(w, 0.12f, 0.08f));
        drawCube(shader, vao, lS, whiteTrim);
    }

    // -------------------------------------------------------------------------
    // Outer Windows (3 Floors for North, South, West & East)
    // -------------------------------------------------------------------------
    int baysZ = 6;
    float stepZ = sideWingDepth / (float)baysZ;

    int baysX = 6;
    float stepX = wingWidth / (float)baysX;

    for (int floor = 0; floor < 5; floor++) {
        float winY = (floor * h1) + (h1 * 0.55f);

        // Outer West Wall (-X) Windows
        float outerWestX = westX - wallThick / 2.0f - 0.02f;
        for (int b = 0; b < baysZ; b++) {
            float winZ = (z - sideWingDepth / 2.0f + stepZ / 2.0f) + b * stepZ;
            glm::mat4 winMat = glm::translate(glm::mat4(1.0f), glm::vec3(outerWestX, winY, winZ));
            winMat = glm::scale(winMat, glm::vec3(0.05f, h1 * 0.40f, stepZ * 0.55f));
            drawCube(shader, vao, winMat, windowGlass);
        }

        // Outer East Wall (+X) Windows (Skipping middle bay for door)
        float outerEastX_Wall = eastX + wallThick / 2.0f + 0.02f;
        for (int b = 0; b < baysZ; b++) {
            float winZ = (z - sideWingDepth / 2.0f + stepZ / 2.0f) + b * stepZ;
            if (std::abs(winZ - z) > entryLengthZ * 0.6f || floor > 0) { // Keep windows on upper floors or away from entrance
                glm::mat4 winMat = glm::translate(glm::mat4(1.0f), glm::vec3(outerEastX_Wall, winY, winZ));
                winMat = glm::scale(winMat, glm::vec3(0.05f, h1 * 0.40f, stepZ * 0.55f));
                drawCube(shader, vao, winMat, windowGlass);
            }
        }

        // Outer North Wall (-Z) Windows
        float outerNorthZ = northZ - wallThick / 2.0f - 0.02f;
        for (int b = 0; b < baysX; b++) {
            float winX = (x - wingWidth / 2.0f + stepX / 2.0f) + b * stepX;
            glm::mat4 winMat = glm::translate(glm::mat4(1.0f), glm::vec3(winX, winY, outerNorthZ));
            winMat = glm::scale(winMat, glm::vec3(stepX * 0.55f, h1 * 0.40f, 0.05f));
            drawCube(shader, vao, winMat, windowGlass);
        }

        // Outer South Wall (+Z) Windows
        float outerSouthZ = southZ + wallThick / 2.0f + 0.02f;
        for (int b = 0; b < baysX; b++) {
            float winX = (x - wingWidth / 2.0f + stepX / 2.0f) + b * stepX;
            glm::mat4 winMat = glm::translate(glm::mat4(1.0f), glm::vec3(winX, winY, outerSouthZ));
            winMat = glm::scale(winMat, glm::vec3(stepX * 0.55f, h1 * 0.40f, 0.05f));
            drawCube(shader, vao, winMat, windowGlass);
        }
    }

    // =========================================================================
    // 4. INNER COURTYARD PILLARS & WINDOWS (ALL 4 WINGS)
    // =========================================================================
    float innerWestX = westX + wallThick / 2.0f + 0.02f;
    float innerEastX = eastX - wallThick / 2.0f - 0.02f;
    float innerNorthZ = northZ + wallThick / 2.0f + 0.02f;
    float innerSouthZ = southZ - wallThick / 2.0f - 0.02f;

    float colOffsetsZ[4] = { z - sideWingDepth * 0.35f, z - sideWingDepth * 0.12f, z + sideWingDepth * 0.12f, z + sideWingDepth * 0.35f };
    float colOffsetsX[4] = { x - wingWidth * 0.35f,     x - wingWidth * 0.12f,     x + wingWidth * 0.12f,     x + wingWidth * 0.35f };

    // --- West Inner Wall ---
    for (int c = 0; c < 5; c++) {
        glm::mat4 colMat = glm::translate(glm::mat4(1.0f), glm::vec3(innerWestX + 0.02f, h / 2.0f, colOffsetsZ[c]));
        colMat = glm::scale(colMat, glm::vec3(0.08f, h, 0.10f));
        drawCube(shader, vao, colMat, whiteTrim);
    }
    for (int f = 0; f < 5; f++) {
        float winY = (f * h1) + (h1 * 0.55f);
        glm::mat4 winWestInner = glm::translate(glm::mat4(1.0f), glm::vec3(innerWestX, winY, z));
        winWestInner = glm::scale(winWestInner, glm::vec3(0.05f, h1 * 0.50f, sideWingDepth * 0.75f));
        drawCube(shader, vao, winWestInner, windowGlass);
    }

    // --- East Inner Wall ---
    for (int c = 0; c < 5; c++) {
        glm::mat4 colMat = glm::translate(glm::mat4(1.0f), glm::vec3(innerEastX - 0.02f, h / 2.0f, colOffsetsZ[c]));
        colMat = glm::scale(colMat, glm::vec3(0.08f, h, 0.10f));
        drawCube(shader, vao, colMat, whiteTrim);
    }
    for (int f = 0; f < 5; f++) {
        float winY = (f * h1) + (h1 * 0.55f);
        glm::mat4 winEastInner = glm::translate(glm::mat4(1.0f), glm::vec3(innerEastX, winY, z));
        winEastInner = glm::scale(winEastInner, glm::vec3(0.05f, h1 * 0.50f, sideWingDepth * 0.75f));
        drawCube(shader, vao, winEastInner, windowGlass);
    }

    // --- North Inner Wall ---
    for (int c = 0; c < 5; c++) {
        glm::mat4 colMat = glm::translate(glm::mat4(1.0f), glm::vec3(colOffsetsX[c], h / 2.0f, innerNorthZ + 0.02f));
        colMat = glm::scale(colMat, glm::vec3(0.10f, h, 0.08f));
        drawCube(shader, vao, colMat, whiteTrim);
    }
    for (int f = 0; f < 5; f++) {
        float winY = (f * h1) + (h1 * 0.55f);
        glm::mat4 winNorthInner = glm::translate(glm::mat4(1.0f), glm::vec3(x, winY, innerNorthZ));
        winNorthInner = glm::scale(winNorthInner, glm::vec3(wingWidth * 0.75f, h1 * 0.50f, 0.05f));
        drawCube(shader, vao, winNorthInner, windowGlass);
    }

    // --- South Inner Wall ---
    for (int c = 0; c < 5; c++) {
        glm::mat4 colMat = glm::translate(glm::mat4(1.0f), glm::vec3(colOffsetsX[c], h / 2.0f, innerSouthZ - 0.02f));
        colMat = glm::scale(colMat, glm::vec3(0.10f, h, 0.08f));
        drawCube(shader, vao, colMat, whiteTrim);
    }
    for (int f = 0; f < 5; f++) {
        float winY = (f * h1) + (h1 * 0.55f);
        glm::mat4 winSouthInner = glm::translate(glm::mat4(1.0f), glm::vec3(x, winY, innerSouthZ));
        winSouthInner = glm::scale(winSouthInner, glm::vec3(wingWidth * 0.75f, h1 * 0.50f, 0.05f));
        drawCube(shader, vao, winSouthInner, windowGlass);
    }
}


//============================================================================================
void drawRoads(Shader& shaderProgram, VAO& vao) {
    glm::vec3 asphalt(0.25f, 0.25f, 0.25f);
    glm::vec3 stripeColor(0.95f, 0.85f, 0.10f);

    glm::mat4 mainRoad = glm::translate(glm::mat4(1.0f), glm::vec3(3.5f, 0.05f, 0.0f));
    mainRoad = glm::scale(mainRoad, glm::vec3(7.0f, 0.02f, 60.0f));
    drawCube(shaderProgram, vao, mainRoad, asphalt);

    for (float z = -28.0f; z <= 28.0f; z += 4.0f) {
        glm::mat4 stripe = glm::translate(glm::mat4(1.0f), glm::vec3(3.5f, 0.07f, z));
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

    float centerX = 12.0f, centerZ = 17.0f;

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





void drawStreetLightPost(Shader& shaderProgram, VAO& vao, float x, float z, bool isLit) {
    // Colors matching image_7a711f.jpg
    glm::vec3 goldColor(0.85f, 0.68f, 0.25f);   // Ornate brass base, scrollwork & finial
    glm::vec3 poleColor(0.18f, 0.18f, 0.20f);   // Dark metallic grey pole
    glm::vec3 shadeColor(0.12f, 0.12f, 0.14f);  // Black bell lamp shades
    glm::vec3 bulbColor = isLit ? glm::vec3(1.0f, 0.96f, 0.65f) : glm::vec3(0.30f, 0.30f, 0.30f); // Glowing light

    auto drawVerticalCylinder = [&](float radius, float height, glm::vec3 pos, glm::vec3 color) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        drawCylinder(shaderProgram, radius, height, model, color);
        };

    // 1. Gold Pedestal Base (Y = 0.0 to 0.8)
    glm::mat4 base1 = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.10f, z));
    base1 = glm::scale(base1, glm::vec3(0.50f, 0.20f, 0.50f));
    drawCube(shaderProgram, vao, base1, goldColor);

    glm::mat4 base2 = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.45f, z));
    base2 = glm::scale(base2, glm::vec3(0.35f, 0.50f, 0.35f));
    drawCube(shaderProgram, vao, base2, goldColor);

    glm::mat4 baseCap = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.75f, z));
    baseCap = glm::scale(baseCap, glm::vec3(0.42f, 0.10f, 0.42f));
    drawCube(shaderProgram, vao, baseCap, goldColor);

    // 2. Central Dark Main Pole (Y = 0.8 to 4.2)
    drawVerticalCylinder(0.08f, 3.40f, glm::vec3(x, 2.48f, z), poleColor);

    // Gold Trim Ring mid-pole
    glm::mat4 ring = glm::translate(glm::mat4(1.0f), glm::vec3(x, 3.80f, z));
    ring = glm::scale(ring, glm::vec3(0.20f, 0.08f, 0.20f));
    drawCube(shaderProgram, vao, ring, goldColor);

    // 3. Ornate Gold Scrollwork Cross-Arms at the Top
    glm::mat4 mainArm = glm::translate(glm::mat4(1.0f), glm::vec3(x, 4.35f, z));
    mainArm = glm::scale(mainArm, glm::vec3(2.20f, 0.12f, 0.12f));
    drawCube(shaderProgram, vao, mainArm, goldColor);

    // Central Ornate Crest Arch
    glm::mat4 crest = glm::translate(glm::mat4(1.0f), glm::vec3(x, 4.55f, z));
    crest = glm::scale(crest, glm::vec3(0.80f, 0.30f, 0.10f));
    drawCube(shaderProgram, vao, crest, goldColor);

    // Top Gold Finial Tip
    glm::mat4 finial = glm::translate(glm::mat4(1.0f), glm::vec3(x, 4.85f, z));
    finial = glm::scale(finial, glm::vec3(0.12f, 0.30f, 0.12f));
    drawCube(shaderProgram, vao, finial, goldColor);

    // 4. Twin Lamp Heads (-X and +X Sides)
    float armOffsetsX[2] = { -1.0f, 1.0f };
    for (int i = 0; i < 2; i++) {
        float lx = x + armOffsetsX[i];

        // Black Bell Lamp Shade
        glm::mat4 shade = glm::translate(glm::mat4(1.0f), glm::vec3(lx, 4.15f, z));
        shade = glm::scale(shade, glm::vec3(0.40f, 0.25f, 0.40f));
        drawCube(shaderProgram, vao, shade, shadeColor);

        // Illuminating Glass Bulb
        glm::mat4 bulb = glm::translate(glm::mat4(1.0f), glm::vec3(lx, 3.95f, z));
        bulb = glm::scale(bulb, glm::vec3(0.24f, 0.20f, 0.24f));
        drawCube(shaderProgram, vao, bulb, bulbColor);

        // Downward Light Beam Cone (Only drawn when spotlight is turned ON)
        if (isLit) {
            glm::vec3 beamColor(1.0f, 0.95f, 0.70f);
            glm::mat4 beam = glm::translate(glm::mat4(1.0f), glm::vec3(lx, 1.95f, z));
            beam = glm::scale(beam, glm::vec3(1.80f, 3.80f, 1.80f));
            drawCube(shaderProgram, vao, beam, beamColor * 0.35f); // Soft spotlight beam effect
        }
    }
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

void drawURPBuilding(Shader& shaderProgram, VAO& vao, float x, float y, float z, float scale = 1.0f) {
    // -------------------------------------------------------------
    // KUET URP BUILDING COLOR PALETTE
    // -------------------------------------------------------------
    glm::vec3 tanConcrete(0.85f, 0.74f, 0.60f);   // Primary beige/tan building finish
    glm::vec3 darkConcrete(0.68f, 0.58f, 0.46f);  // Inner recessed walls & trims
    glm::vec3 whitePillar(0.92f, 0.93f, 0.95f);   // White cylindrical columns
    glm::vec3 glassWindow(0.18f, 0.22f, 0.28f);   // Tinted window glass
    glm::vec3 stairWhite(0.88f, 0.88f, 0.86f);    // Grand staircase steps
    glm::vec3 stairRailing(0.55f, 0.15f, 0.12f);  // Terracotta/red staircase side walls
    glm::vec3 louverColor(0.78f, 0.66f, 0.52f);   // Vertical sunscreen louvers
    glm::vec3 railingColor(0.70f, 0.72f, 0.75f);  // Balcony railings
    glm::vec3 waterTankColor(0.12f, 0.14f, 0.18f); // Roof water tanks

    glm::mat4 baseMat = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    baseMat = glm::scale(baseMat, glm::vec3(scale, scale, scale));

    // -------------------------------------------------------------
    // 1. MAIN REAR & SIDE WING FRAME
    // -------------------------------------------------------------
    // Main Rear Mass (5 stories tall)
    glm::mat4 mainMass = glm::translate(baseMat, glm::vec3(0.0f, 7.5f, -6.0f));
    mainMass = glm::scale(mainMass, glm::vec3(22.0f, 15.0f, 6.0f));
    drawCube(shaderProgram, vao, mainMass, tanConcrete);

    // Right Solid Wing (Front right block with clean face)
    glm::mat4 rightWing = glm::translate(baseMat, glm::vec3(8.5f, 7.5f, 0.0f));
    rightWing = glm::scale(rightWing, glm::vec3(5.0f, 15.0f, 6.0f));
    drawCube(shaderProgram, vao, rightWing, tanConcrete);

    // Left Wing Backing Block
    glm::mat4 leftWing = glm::translate(baseMat, glm::vec3(-8.5f, 7.5f, -1.0f));
    leftWing = glm::scale(leftWing, glm::vec3(5.0f, 15.0f, 14.0f));
    drawCube(shaderProgram, vao, leftWing, tanConcrete);

    // -------------------------------------------------------------
    // 2. RIGHT WING VERTICAL SLIT WINDOWS
    // -------------------------------------------------------------
    for (int i = 0; i < 4; i++) {
        glm::mat4 slitWin = glm::translate(baseMat, glm::vec3(8.5f, 3.0f + i * 3.2f, 3.01f));
        slitWin = glm::scale(slitWin, glm::vec3(0.8f, 1.8f, 0.05f));
        drawCube(shaderProgram, vao, slitWin, glassWindow);
    }

    // -------------------------------------------------------------
    // 3. CENTRAL ATRIUM BALCONY SLABS & CORRIDORS
    // -------------------------------------------------------------
    const int NUM_FLOORS = 5;
    float floorHeight = 2.8f;

    for (int i = 0; i < NUM_FLOORS; i++) {
        float fY = 1.2f + i * floorHeight;

        // Atrium Corridor Floor Slab
        glm::mat4 floorSlab = glm::translate(baseMat, glm::vec3(0.0f, fY, -1.0f));
        floorSlab = glm::scale(floorSlab, glm::vec3(12.0f, 0.4f, 6.0f));
        drawCube(shaderProgram, vao, floorSlab, tanConcrete);

        // Recessed Back Wall Windows per floor
        glm::mat4 backWin = glm::translate(baseMat, glm::vec3(0.0f, fY + 1.2f, -3.95f));
        backWin = glm::scale(backWin, glm::vec3(11.0f, 1.8f, 0.05f));
        drawCube(shaderProgram, vao, backWin, glassWindow);

        // Balcony Silver Railings
        if (i > 0) {
            glm::mat4 railing = glm::translate(baseMat, glm::vec3(0.0f, fY + 0.6f, 1.95f));
            railing = glm::scale(railing, glm::vec3(11.8f, 0.8f, 0.08f));
            drawCube(shaderProgram, vao, railing, railingColor);
        }
    }

    // -------------------------------------------------------------
    // 4. FRONT ATRIUM STRUCTURAL PILLARS (White Cylinders)
    // -------------------------------------------------------------
    float pillarX[] = { -5.2f, -1.8f, 1.8f, 5.2f };
    for (int i = 0; i < 4; i++) {
        glm::mat4 pMat = glm::translate(baseMat, glm::vec3(pillarX[i], 0.0f, 1.90f));
        // Using drawCylinder for smooth structural columns
        drawCylinder(shaderProgram, 0.25f, 13.5f, pMat, whitePillar);
    }

    // -------------------------------------------------------------
    // 5. GRAND CENTRAL ENTRANCE STAIRCASE
    // -------------------------------------------------------------
    int numSteps = 16;
    float stepWidth = 6.5f;
    float stepDepth = 0.35f;
    float stepHeight = 0.20f;
    float stairStartZ = 2.2f;

    for (int i = 0; i < numSteps; i++) {
        glm::mat4 step = glm::translate(baseMat, glm::vec3(-0.5f, i * stepHeight + 0.1f, stairStartZ - i * stepDepth));
        step = glm::scale(step, glm::vec3(stepWidth, stepHeight, stepDepth));
        drawCube(shaderProgram, vao, step, stairWhite);
    }

    // Staircase Terracotta/Red Side Walls
    glm::mat4 stairWallL = glm::translate(baseMat, glm::vec3(-0.5f - stepWidth * 0.5f - 0.15f, 1.6f, stairStartZ - (numSteps * stepDepth * 0.5f)));
    stairWallL = glm::scale(stairWallL, glm::vec3(0.3f, 2.8f, numSteps * stepDepth));
    drawCube(shaderProgram, vao, stairWallL, stairRailing);

    glm::mat4 stairWallR = glm::translate(baseMat, glm::vec3(-0.5f + stepWidth * 0.5f + 0.15f, 1.6f, stairStartZ - (numSteps * stepDepth * 0.5f)));
    stairWallR = glm::scale(stairWallR, glm::vec3(0.3f, 2.8f, numSteps * stepDepth));
    drawCube(shaderProgram, vao, stairWallR, stairRailing);

    // -------------------------------------------------------------
    // 6. LEFT FACADE VERTICAL SUNSCREEN LOUVERS
    // -------------------------------------------------------------
    const int NUM_LOUVERS = 14;
    float louverStartX = -10.8f;
    float louverSpacing = 0.35f;

    for (int i = 0; i < NUM_LOUVERS; i++) {
        // Lower Section Louver Screen
        glm::mat4 louver = glm::translate(baseMat, glm::vec3(louverStartX + i * louverSpacing, 4.2f, 5.95f));
        louver = glm::scale(louver, glm::vec3(0.08f, 6.2f, 0.30f));
        drawCube(shaderProgram, vao, louver, louverColor);
    }

    // Upper Left Window Grids
    for (int floor = 3; floor < 5; floor++) {
        for (int bay = 0; bay < 6; bay++) {
            glm::mat4 gridWin = glm::translate(baseMat, glm::vec3(-10.5f + bay * 0.8f, 1.2f + floor * floorHeight, 5.95f));
            gridWin = glm::scale(gridWin, glm::vec3(0.65f, 1.8f, 0.05f));
            drawCube(shaderProgram, vao, gridWin, glassWindow);
        }
    }

    // -------------------------------------------------------------
    // 7. ROOFTOP TOWER & WATER TANKS
    // -------------------------------------------------------------
    // Elevator Shaft Tower Extension
    glm::mat4 roofTower = glm::translate(baseMat, glm::vec3(-4.5f, 15.8f, -4.0f));
    roofTower = glm::scale(roofTower, glm::vec3(3.5f, 1.8f, 3.5f));
    drawCube(shaderProgram, vao, roofTower, tanConcrete);

    // Black Rooftop Water Tanks (Gazi Tanks from photograph)
    
}



void drawXZRedDots(Shader& shaderProgram, VAO& vao, float gridRange = 35.0f, float step = 2.0f) {
    glm::vec3 redColor(1.0f, 0.0f, 0.0f); // Pure Red

    for (float x = -gridRange; x <= gridRange; x += step) {
        for (float z = -gridRange; z <= gridRange; z += step) {
            // Flat 2D square on X-Z plane at ground level (Y = 0.15f)
            if (x <= 1.0f && z <= 1.0f) {
                glm::mat4 dotMat = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.15f, z));
                dotMat = glm::scale(dotMat, glm::vec3(0.18f, 10.01f, 0.18f));
                drawCube(shaderProgram, vao, dotMat, redColor);
                continue;
            }
            glm::mat4 dotMat = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.15f, z));
            dotMat = glm::scale(dotMat, glm::vec3(0.18f, 0.01f, 0.18f));
            drawCube(shaderProgram, vao, dotMat, redColor);
        }
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark(); // Sets a nice dark theme

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    Shader shaderProgram("default.vert", "default.frag");
    // Sun position in world space
    glm::vec3 sunPos(25.0f, 45.0f, 25.0f);

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


        // Dynamic Sky Background Color (Day vs Night)
        if (isDayMode) {
            glClearColor(0.45f, 0.70f, 0.90f, 1.0f); // Bright blue sky
        }
        else {
            glClearColor(0.05f, 0.05f, 0.15f, 1.0f); // Dark night sky
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // 1. Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();



        shaderProgram.Activate();
        glUniform1i(glGetUniformLocation(shaderProgram.ID, "shadingMode"), currentShadingMode);

        // Pass lighting uniforms based on Day/Night state
        float ambientVal = isDayMode ? 0.45f : 0.08f;
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "ambientStrength"), ambientVal);

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


        // Sun position & lighting
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), sunPos.x, sunPos.y, sunPos.z);

        // Ground Plane
        glm::mat4 ground = glm::scale(glm::mat4(1.0f), glm::vec3(70.0f, 0.1f, 70.0f));
        drawCube(shaderProgram, VAO1, ground, glm::vec3(0.25f, 0.55f, 0.22f));


        //==================================================
        //==================================================


      // Ground Plane
        glm::mat4 ground2 = glm::scale(glm::mat4(1.0f), glm::vec3(150.0f, 0.1f, 150.0f));
        drawCube(shaderProgram, VAO1, ground2, glm::vec3(0.25f, 0.55f, 0.22f));

        //sun===================================================
      // Draw Sun (Only visible brightly during Day mode)
        glm::mat4 sunModel = glm::translate(glm::mat4(1.0f), sunPos);
        sunModel = glm::scale(sunModel, glm::vec3(2.5f, 2.5f, 2.5f));
        glm::vec3 sunColor = isDayMode ? glm::vec3(1.0f, 0.95f, 0.2f) : glm::vec3(0.4f, 0.4f, 0.5f);
        drawCube(shaderProgram, VAO1, sunModel, sunColor);
        //=============================================================

        // Draw red dot at every (X, Z) coordinate step of 2.0 units
        drawXZRedDots(shaderProgram, VAO1, 35.0f, 2.0f);


        //=================================================
        //=================================================

        // Academic Blocks
        drawFourConnectedBlocks(shaderProgram, VAO1, 15.0f);

        // Halls
        drawSSHAKH(shaderProgram, VAO1, -22.0f, -5.0f, 12.0f, 12.0f, 5.83f, 1.5f, glm::vec3(0.80f, 0.70f, 0.60f)); // AEH
        drawSSHAKH(shaderProgram, VAO1, -22.0f, 12.0f, 12.0f, 15.0f, 5.83f, 1.8f, glm::vec3(0.80f, 0.70f, 0.60f));  // SSH
        drawFFHDRH(shaderProgram, VAO1, -11.0f, 3.0f, 7.0f, 10.0f, 3.75f, 1.2f, glm::vec3(0.80f, 0.70f, 0.60f));      // FFH
        drawFFHDRH(shaderProgram, VAO1, -11.0f, 23.0f, 7.0f, 10.0f, 3.75f, 1.2f, glm::vec3(0.80f, 0.70f, 0.60f));     // DRH

        // Roads & Roundabout
        drawRoads(shaderProgram, VAO1);
        drawRoundabout(shaderProgram, VAO1);

        // Trees
        float treeZ[] = { -22.0f, -14.0f, -6.0f, 2.0f, 10.0f, 18.0f };
        for (float z : treeZ) {
            drawTree(shaderProgram, VAO1, -3.2f, z);
            drawTree(shaderProgram, VAO1, 8.8f, z);
        }
        drawTree(shaderProgram, VAO1, 16.0f, 22.0f);
        drawTree(shaderProgram, VAO1, 16.0f, 31.0f);


        float lightZ[] = { -25.0f, -10.0f, 2.0f, 14.0f, 26.0f };
        for (float z : lightZ) {
            drawStreetLightPost(shaderProgram, VAO1, -0.5f, z, spotlightOn); // Left road side
            drawStreetLightPost(shaderProgram, VAO1, 7.5f, z, spotlightOn); // Right road side
        }

     

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
        drawCar(shaderProgram, VAO1, (dirZ > 0 ? -0.1f : 3.2f), -vehicleZ, -dirZ, -carWheelRot);
        drawBus(shaderProgram, VAO1, (dirZ > 0 ? 3.2f : -0.1f), vehicleZ, dirZ, busWheelRot);


        //===============================================================================

        // Render KUET URP Building along the side of the road
      // drawURPBuilding(shaderProgram, VAO1, -12.0f, 0.0f, -5.0f, 1.0f);


        // 2. Create an on-screen control window
        ImGui::Begin("Shading Controls");
        ImGui::Text("Select Shading Mode:");

        if (ImGui::Button("Flat Shading")) {
            currentShadingMode = 0;
        }
        if (ImGui::Button("Gouraud Shading")) {
            currentShadingMode = 1;
        }
        if (ImGui::Button("Phong Shading")) {
            currentShadingMode = 2;
        }

        ImGui::Separator();
        // Display active mode status
        const char* modeStrs[] = { "Flat", "Gouraud", "Phong" };
        ImGui::Text("Active: %s", modeStrs[currentShadingMode]);
        ImGui::End();

        // 3. Render ImGui draw data (place this near the end of the loop, right before glfwSwapBuffers)
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    VAO1.Delete();
    VBO1.Delete();
    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}