#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

// --- Minimal Math Library (Replacing GLM) ---
const float PI = 3.14159265359f;

struct Vec3 {
    float x, y, z;
    Vec3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
};

Vec3 subtract(Vec3 a, Vec3 b) { return Vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
Vec3 cross(Vec3 a, Vec3 b) {
    return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
float dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
Vec3 normalize(Vec3 v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return (len > 0) ? Vec3(v.x / len, v.y / len, v.z / len) : v;
}

struct Mat4 {
    float m[16]; // Column-major order
    Mat4() {
        for (int i = 0; i < 16; i++) m[i] = 0.0f;
    }
};

Mat4 identity() {
    Mat4 res;
    res.m[0] = 1; res.m[5] = 1; res.m[10] = 1; res.m[15] = 1;
    return res;
}

Mat4 multiply(Mat4 a, Mat4 b) {
    Mat4 res;
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            res.m[col * 4 + row] =
                a.m[0 * 4 + row] * b.m[col * 4 + 0] +
                a.m[1 * 4 + row] * b.m[col * 4 + 1] +
                a.m[2 * 4 + row] * b.m[col * 4 + 2] +
                a.m[3 * 4 + row] * b.m[col * 4 + 3];
        }
    }
    return res;
}

Mat4 translate(Mat4 mat, Vec3 v) {
    Mat4 res = mat;
    res.m[12] = mat.m[0] * v.x + mat.m[4] * v.y + mat.m[8] * v.z + mat.m[12];
    res.m[13] = mat.m[1] * v.x + mat.m[5] * v.y + mat.m[9] * v.z + mat.m[13];
    res.m[14] = mat.m[2] * v.x + mat.m[6] * v.y + mat.m[10] * v.z + mat.m[14];
    res.m[15] = mat.m[3] * v.x + mat.m[7] * v.y + mat.m[11] * v.z + mat.m[15];
    return res;
}

Mat4 scale(Mat4 mat, Vec3 v) {
    Mat4 res;
    for (int i = 0; i < 4; i++) res.m[i] = mat.m[i] * v.x;
    for (int i = 0; i < 4; i++) res.m[i + 4] = mat.m[i + 4] * v.y;
    for (int i = 0; i < 4; i++) res.m[i + 8] = mat.m[i + 8] * v.z;
    for (int i = 0; i < 4; i++) res.m[i + 12] = mat.m[i + 12];
    return res;
}

Mat4 lookAt(Vec3 eye, Vec3 center, Vec3 up) {
    Vec3 f = normalize(subtract(center, eye));
    Vec3 s = normalize(cross(f, up));
    Vec3 u = cross(s, f);

    Mat4 res = identity();
    res.m[0] = s.x;  res.m[4] = s.y;  res.m[8] = s.z;
    res.m[1] = u.x;  res.m[5] = u.y;  res.m[9] = u.z;
    res.m[2] = -f.x; res.m[6] = -f.y; res.m[10] = -f.z;
    res.m[12] = -dot(s, eye);
    res.m[13] = -dot(u, eye);
    res.m[14] = dot(f, eye);
    return res;
}

Mat4 perspective(float fovRad, float aspect, float zNear, float zFar) {
    Mat4 res;
    float tanHalfFov = std::tan(fovRad / 2.0f);
    res.m[0] = 1.0f / (aspect * tanHalfFov);
    res.m[5] = 1.0f / tanHalfFov;
    res.m[10] = -(zFar + zNear) / (zFar - zNear);
    res.m[11] = -1.0f;
    res.m[14] = -(2.0f * zFar * zNear) / (zFar - zNear);
    return res;
}


// --- Window Globals ---
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;
float cameraAngle = 0.0f;

// --- GLSL Shaders ---
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;

    out vec3 FragPos;
    out vec3 Normal;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main() {
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;  
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;

    in vec3 FragPos;
    in vec3 Normal;

    uniform vec3 objectColor;
    uniform vec3 lightPos;

    void main() {
        float ambientStrength = 0.4;
        vec3 ambient = ambientStrength * vec3(1.0);
        
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * vec3(1.0);
        
        vec3 result = (ambient + diffuse) * objectColor;
        FragColor = vec4(result, 1.0);
    }
)";

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraAngle -= 0.02f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraAngle += 0.02f;
}

unsigned int createShaderProgram() {
    unsigned int vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vShader);

    unsigned int fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fShader);

    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vShader);
    glAttachShader(prog, fShader);
    glLinkProgram(prog);

    glDeleteShader(vShader);
    glDeleteShader(fShader);
    return prog;
}

void drawCube(unsigned int shaderProgram, unsigned int VAO, Mat4 model, Vec3 color) {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, model.m);
    glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), color.x, color.y, color.z);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawBuildingBlock(unsigned int shaderProgram, unsigned int VAO, float xOffset) {
    Vec3 mainColor(0.93f, 0.88f, 0.80f);
    Vec3 balconyColor(0.75f, 0.65f, 0.55f);

    // Main Volume
    Mat4 model = identity();
    model = translate(model, Vec3(xOffset, 4.0f, -2.0f));
    model = scale(model, Vec3(4.0f, 8.0f, 4.0f));
    drawCube(shaderProgram, VAO, model, mainColor);

    // Horizontal Balconies
    for (int i = 1; i <= 5; i++) {
        Mat4 bModel = identity();
        bModel = translate(bModel, Vec3(xOffset, i * 1.5f, -1.8f));
        bModel = scale(bModel, Vec3(4.2f, 0.2f, 4.2f));
        drawCube(shaderProgram, VAO, bModel, balconyColor);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Kuet Building - No GLM", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    unsigned int shaderProgram = createShaderProgram();

    // Cube vertices (Positions + Normals)
    float vertices[] = {
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

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        float radius = 15.0f;
        Vec3 cameraPos(std::sin(cameraAngle) * radius, 5.0f, std::cos(cameraAngle) * radius);

        Mat4 view = lookAt(cameraPos, Vec3(0.0f, 4.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
        Mat4 projection = perspective(45.0f * (PI / 180.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, view.m);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, projection.m);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 10.0f, 15.0f, 10.0f);

        // Ground
        Mat4 groundModel = identity();
        groundModel = scale(groundModel, Vec3(30.0f, 0.1f, 30.0f));
        drawCube(shaderProgram, VAO, groundModel, Vec3(0.3f, 0.5f, 0.2f));

        // Left and Right Blocks
        drawBuildingBlock(shaderProgram, VAO, -3.0f);
        drawBuildingBlock(shaderProgram, VAO, 3.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}