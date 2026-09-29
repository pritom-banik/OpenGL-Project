#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"

const unsigned int width = 1024;
const unsigned int height = 768;
float cameraAngle = 0.0f;

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

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraAngle -= 0.02f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraAngle += 0.02f;
}

void drawCube(Shader& shaderProgram, VAO& vao, glm::mat4 model, glm::vec3 color) {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "objectColor"), color.x, color.y, color.z);

    vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawBuildingBlock(Shader& shaderProgram, VAO& vao, float xOffset) {
    glm::vec3 mainColor(0.93f, 0.88f, 0.80f);
    glm::vec3 balconyColor(0.75f, 0.65f, 0.55f);

    // Main Volume
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(xOffset, 4.0f, -2.0f));
    model = glm::scale(model, glm::vec3(4.0f, 8.0f, 4.0f));
    drawCube(shaderProgram, vao, model, mainColor);

    // Horizontal Balconies
    for (int i = 1; i <= 5; i++) {
        glm::mat4 bModel = glm::mat4(1.0f);
        bModel = glm::translate(bModel, glm::vec3(xOffset, i * 1.5f, -1.8f));
        bModel = glm::scale(bModel, glm::vec3(4.2f, 0.2f, 4.2f));
        drawCube(shaderProgram, vao, bModel, balconyColor);
    }
}

void drawRoad(Shader& shaderProgram, VAO& vao) {
    glm::vec3 roadColor(0.2f, 0.2f, 0.2f);   // Asphalt
    glm::vec3 stripeColor(0.9f, 0.8f, 0.1f); // Yellow lane stripes

    // Main Road Surface (positioned slightly above ground level)
    glm::mat4 roadModel = glm::mat4(1.0f);
    roadModel = glm::translate(roadModel, glm::vec3(0.0f, 0.06f, 1.0f));
    roadModel = glm::scale(roadModel, glm::vec3(30.0f, 0.02f, 2.5f));
    drawCube(shaderProgram, vao, roadModel, roadColor);

    // Dashed Center Markings
    for (float x = -13.0f; x <= 13.0f; x += 3.0f) {
        glm::mat4 stripeModel = glm::mat4(1.0f);
        stripeModel = glm::translate(stripeModel, glm::vec3(x, 0.075f, 1.0f));
        stripeModel = glm::scale(stripeModel, glm::vec3(1.5f, 0.02f, 0.2f));
        drawCube(shaderProgram, vao, stripeModel, stripeColor);
    }
}

void drawTree(Shader& shaderProgram, VAO& vao, float xOffset, float zOffset) {
    glm::vec3 trunkColor(0.40f, 0.25f, 0.15f);
    glm::vec3 leavesColor(0.15f, 0.55f, 0.20f);

    // Trunk
    glm::mat4 trunkModel = glm::mat4(1.0f);
    trunkModel = glm::translate(trunkModel, glm::vec3(xOffset, 1.25f, zOffset));
    trunkModel = glm::scale(trunkModel, glm::vec3(0.5f, 2.5f, 0.5f));
    drawCube(shaderProgram, vao, trunkModel, trunkColor);

    // Foliage Layer 1
    glm::mat4 leavesModel1 = glm::mat4(1.0f);
    leavesModel1 = glm::translate(leavesModel1, glm::vec3(xOffset, 3.0f, zOffset));
    leavesModel1 = glm::scale(leavesModel1, glm::vec3(2.2f, 1.5f, 2.2f));
    drawCube(shaderProgram, vao, leavesModel1, leavesColor);

    // Foliage Layer 2
    glm::mat4 leavesModel2 = glm::mat4(1.0f);
    leavesModel2 = glm::translate(leavesModel2, glm::vec3(xOffset, 4.2f, zOffset));
    leavesModel2 = glm::scale(leavesModel2, glm::vec3(1.5f, 1.2f, 1.5f));
    drawCube(shaderProgram, vao, leavesModel2, leavesColor);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Building, Road & Trees", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, width, height);

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

        glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Activate();

        float radius = 18.0f;
        glm::vec3 cameraPos(std::sin(cameraAngle) * radius, 6.0f, std::cos(cameraAngle) * radius);

        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 3.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), 10.0f, 15.0f, 10.0f);

        // Ground
        glm::mat4 groundModel = glm::mat4(1.0f);
        groundModel = glm::scale(groundModel, glm::vec3(30.0f, 0.1f, 30.0f));
        drawCube(shaderProgram, VAO1, groundModel, glm::vec3(0.3f, 0.5f, 0.2f));

        // Buildings (z = -2.0)
        drawBuildingBlock(shaderProgram, VAO1, -3.0f);
        drawBuildingBlock(shaderProgram, VAO1, 3.0f);

        // Road (z = 1.0)
        drawRoad(shaderProgram, VAO1);

        // Trees on the far side of the road (z = 3.5)
        drawTree(shaderProgram, VAO1, -7.0f, 3.5f);
        drawTree(shaderProgram, VAO1, 0.0f, 3.5f);
        drawTree(shaderProgram, VAO1, 7.0f, 3.5f);

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