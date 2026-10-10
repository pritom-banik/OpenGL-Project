#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 GouraudAmbient;
out vec3 GouraudDirectional;
out vec3 GouraudPoint;
out vec3 GouraudSpot;
out vec3 GouraudArea;
out vec3 GouraudHallWindow;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform vec3 directionalLightDirection;
uniform vec3 shadowLightDirection;
uniform vec3 directionalLightColor;
uniform vec3 pointLightPosition;
uniform vec3 pointLightColor;
uniform vec3 spotLightPositions[26];
uniform vec3 spotLightDirections[26];
uniform vec3 spotLightColor;
uniform vec3 areaLightPositions[4];
uniform vec3 areaLightColor;
uniform vec3 hallWindowLightPositions[20];
uniform vec3 hallWindowLightColor;
uniform float ambientStrength;
uniform float spotInnerCutoff;
uniform float spotOuterCutoff;
uniform int shadingMode;
uniform int pointLightEnabled;
uniform int directionalLightEnabled;
uniform int spotLightEnabled;
uniform int areaLightEnabled;
uniform int hallWindowLightsEnabled;
uniform int shadowSourceType;
uniform int shadowPass;

vec3 illuminate(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 lightColor, float attenuation) {
    float diffuse = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specular = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    return (diffuse + 0.25 * specular) * lightColor * attenuation;
}

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    Normal = mat3(transpose(inverse(model))) * aNormal;

    if (shadowPass == 1) {
        gl_Position = lightSpaceMatrix * worldPos;
        return;
    }

    if (shadingMode == 1) {
        vec3 normal = normalize(Normal);
        vec3 viewDir = normalize(viewPos - FragPos);
        GouraudAmbient = ambientStrength * vec3(1.0);
        GouraudDirectional = vec3(0.0);
        GouraudPoint = vec3(0.0);
        GouraudSpot = vec3(0.0);
        GouraudArea = vec3(0.0);
        GouraudHallWindow = vec3(0.0);

        if (directionalLightEnabled == 1) {
            GouraudDirectional += illuminate(normal, normalize(-directionalLightDirection), viewDir, directionalLightColor, 1.0);
        }
        if (pointLightEnabled == 1) {
            vec3 delta = pointLightPosition - FragPos;
            float distanceToLight = length(delta);
            float attenuation = 1.0 / (1.0 + 0.02 * distanceToLight + 0.002 * distanceToLight * distanceToLight);
            GouraudPoint += illuminate(normal, normalize(delta), viewDir, pointLightColor, attenuation);
        }
        if (spotLightEnabled == 1) {
            for (int i = 0; i < 26; ++i) {
                vec3 delta = spotLightPositions[i] - FragPos;
                float distanceToLight = length(delta);
                float theta = dot(normalize(-delta), normalize(spotLightDirections[i]));
                float cone = clamp((theta - spotOuterCutoff) / (spotInnerCutoff - spotOuterCutoff), 0.0, 1.0);
                float attenuation = cone / (1.0 + 0.04 * distanceToLight + 0.01 * distanceToLight * distanceToLight);
                GouraudSpot += illuminate(normal, normalize(delta), viewDir, spotLightColor, attenuation);
            }
        }
        if (areaLightEnabled == 1) {
            for (int i = 0; i < 4; ++i) {
                vec3 delta = areaLightPositions[i] - FragPos;
                float distanceToLight = length(delta);
                float attenuation = 0.25 / (1.0 + 0.02 * distanceToLight + 0.002 * distanceToLight * distanceToLight);
                GouraudArea += illuminate(normal, normalize(delta), viewDir, areaLightColor, attenuation);
            }
        }
        if (hallWindowLightsEnabled == 1) {
            for (int i = 0; i < 20; ++i) {
                vec3 delta = hallWindowLightPositions[i] - FragPos;
                float distanceToLight = length(delta);
                float attenuation = 0.05 / (1.0 + 0.04 * distanceToLight + 0.008 * distanceToLight * distanceToLight);
                GouraudHallWindow += illuminate(normal, normalize(delta), viewDir, hallWindowLightColor, attenuation);
            }
        }
        GouraudAmbient *= objectColor;
        GouraudDirectional *= objectColor;
        GouraudPoint *= objectColor;
        GouraudSpot *= objectColor;
        GouraudArea *= objectColor;
        GouraudHallWindow *= objectColor;
    }

    gl_Position = proj * view * worldPos;
}