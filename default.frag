#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightPos;

void main() {
    // Ambient light
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * vec3(1.0);
    
    // Diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);
    
    // Combine lighting with object color
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}