#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
flat out vec3 FlatFragPos;
flat out vec3 FlatNormal;
out vec3 Normal;
out vec3 GouraudColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform int shadingMode;
uniform float ambientStrength; // Dynamic ambient for Day/Night

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    Normal = mat3(transpose(inverse(model))) * aNormal;

    if (shadingMode == 0) {
        FlatFragPos = FragPos;
        FlatNormal = Normal;
    } 
    else if (shadingMode == 1) {
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        
        // Softer attenuation for large scale
        float distance = length(lightPos - FragPos);
        float attenuation = 1.0 / (1.0 + 0.001 * distance + 0.00005 * (distance * distance));

        vec3 ambient = ambientStrength * vec3(1.0);
        vec3 diffuse = diff * vec3(1.0) * attenuation;
        
        GouraudColor = (ambient + diffuse) * objectColor;
    }

    gl_Position = proj * view * worldPos;
}