#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 GouraudAmbient;
in vec3 GouraudDirectional;
in vec3 GouraudPoint;
in vec3 GouraudSpot;
in vec3 GouraudArea;
in vec3 GouraudHallWindow;

uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform vec3 directionalLightDirection;
uniform vec3 shadowLightDirection;
uniform vec3 directionalLightColor;
uniform vec3 pointLightPosition;
uniform vec3 pointLightColor;
uniform vec3 spotLightPositions[20];
uniform vec3 spotLightDirections[20];
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
uniform int shadowsEnabled;
uniform int shadowSourceType;
uniform int shadowPass;
uniform int emissiveObject;
uniform float objectAlpha;

vec3 illuminate(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 lightColor, float attenuation) {
    float diffuse = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specular = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    return (diffuse + 0.25 * specular) * lightColor * attenuation;
}

float calculateShadow(vec3 normal) {
    if (shadowsEnabled == 0 || shadowSourceType == 0) return 0.0;

    vec4 lightClip = lightSpaceMatrix * vec4(FragPos, 1.0);
    vec3 projected = lightClip.xyz / lightClip.w;
    projected = projected * 0.5 + 0.5;
    if (projected.z < 0.0 || projected.z > 1.0 || projected.x < 0.0 || projected.x > 1.0 || projected.y < 0.0 || projected.y > 1.0) return 0.0;

    float currentDepth = projected.z;
    float bias = max(0.0015 * (1.0 - dot(normal, normalize(-shadowLightDirection))), 0.0005);
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    float shadow = 0.0;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float closestDepth = texture(shadowMap, projected.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

void main() {
    if (shadowPass == 1) {
        FragColor = vec4(1.0);
        return;
    }
    if (emissiveObject == 1) {
        FragColor = vec4(objectColor, objectAlpha);
        return;
    }

    vec3 normal;
    if (shadingMode == 0) {
        normal = normalize(cross(dFdx(FragPos), dFdy(FragPos)));
        if (!gl_FrontFacing) normal = -normal;
    }
    else {
        normal = normalize(Normal);
    }

    float shadow = calculateShadow(normal);
    float shadowVisibility = 1.0 - shadow;
    vec3 result;
    if (shadingMode == 1) {
        result = GouraudAmbient
            + GouraudDirectional * (shadowSourceType == 1 ? shadowVisibility : 1.0)
            + GouraudPoint * (shadowSourceType == 3 ? shadowVisibility : 1.0)
            + GouraudSpot * (shadowSourceType == 2 ? shadowVisibility : 1.0)
            + GouraudArea * (shadowSourceType == 4 ? shadowVisibility : 1.0)
            + GouraudHallWindow * (shadowSourceType == 5 ? shadowVisibility : 1.0);
    }
    else {
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 ambient = ambientStrength * vec3(1.0);
        vec3 directional = vec3(0.0);
        vec3 pointLights = vec3(0.0);
        vec3 spotLights = vec3(0.0);
        vec3 areaLights = vec3(0.0);
        vec3 hallWindowLights = vec3(0.0);

        if (directionalLightEnabled == 1) {
            directional = illuminate(normal, normalize(-directionalLightDirection), viewDir, directionalLightColor, 1.0);
        }
        if (pointLightEnabled == 1) {
            vec3 delta = pointLightPosition - FragPos;
            float distanceToLight = length(delta);
            float attenuation = 1.0 / (1.0 + 0.02 * distanceToLight + 0.002 * distanceToLight * distanceToLight);
            pointLights += illuminate(normal, normalize(delta), viewDir, pointLightColor, attenuation);
        }
        if (spotLightEnabled == 1) {
            for (int i = 0; i < 20; ++i) {
                vec3 delta = spotLightPositions[i] - FragPos;
                float distanceToLight = length(delta);
                float theta = dot(normalize(-delta), normalize(spotLightDirections[i]));
                float cone = clamp((theta - spotOuterCutoff) / (spotInnerCutoff - spotOuterCutoff), 0.0, 1.0);
                float attenuation = cone / (1.0 + 0.04 * distanceToLight + 0.01 * distanceToLight * distanceToLight);
                spotLights += illuminate(normal, normalize(delta), viewDir, spotLightColor, attenuation);
            }
        }
        if (areaLightEnabled == 1) {
            for (int i = 0; i < 4; ++i) {
                vec3 delta = areaLightPositions[i] - FragPos;
                float distanceToLight = length(delta);
                float attenuation = 0.25 / (1.0 + 0.02 * distanceToLight + 0.002 * distanceToLight * distanceToLight);
                areaLights += illuminate(normal, normalize(delta), viewDir, areaLightColor, attenuation);
            }
        }
        if (hallWindowLightsEnabled == 1) {
            for (int i = 0; i < 20; ++i) {
                vec3 delta = hallWindowLightPositions[i] - FragPos;
                float distanceToLight = length(delta);
                float attenuation = 0.05 / (1.0 + 0.04 * distanceToLight + 0.008 * distanceToLight * distanceToLight);
                hallWindowLights += illuminate(normal, normalize(delta), viewDir, hallWindowLightColor, attenuation);
            }
        }
        result = (ambient
            + directional * (shadowSourceType == 1 ? shadowVisibility : 1.0)
            + pointLights * (shadowSourceType == 3 ? shadowVisibility : 1.0)
            + spotLights * (shadowSourceType == 2 ? shadowVisibility : 1.0)
            + areaLights * (shadowSourceType == 4 ? shadowVisibility : 1.0)
            + hallWindowLights * (shadowSourceType == 5 ? shadowVisibility : 1.0)) * objectColor;
    }

    FragColor = vec4(result, objectAlpha);
}