#version 330 core
out vec4 FragColor;

in vec3 FragPos;
flat in vec3 FlatFragPos;
flat in vec3 FlatNormal;
in vec3 Normal;
in vec3 GouraudColor;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform int shadingMode;
uniform vec3 viewPos;
uniform float ambientStrength; // Dynamic ambient for Day/Night

void main() {
    vec3 ambient = ambientStrength * vec3(1.0);

    // Softer distance attenuation
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.001 * distance + 0.00005 * (distance * distance));

    if (shadingMode == 0) {
        vec3 norm = normalize(FlatNormal);
        vec3 lightDir = normalize(lightPos - FlatFragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * vec3(1.0) * attenuation;
        
        FragColor = vec4((ambient + diffuse) * objectColor, 1.0);
    } 
    else if (shadingMode == 1) {
        FragColor = vec4(GouraudColor, 1.0);
    } 
    else {
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * vec3(1.0) * attenuation;

        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = 0.5 * spec * vec3(1.0) * attenuation;

        vec3 result = (ambient + diffuse + specular) * objectColor;
        FragColor = vec4(result, 1.0);
    }
}