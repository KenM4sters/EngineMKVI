#version 450

layout(location = 0) out vec4 aColor;
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
    vec4 ambientLightColor;
    vec3 pointLightPosition;
    vec4 pointLightColor;

} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix; // projection * view * model
    mat4 normalMatrix;
} push;


void main() {

    vec3 directionToLight = ubo.pointLightPosition - fragPosWorld;
    float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance^2

    vec3 pointLightColor = ubo.pointLightColor.xyz * ubo.pointLightColor.w * attenuation;
    vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;

    vec3 diffuseLight = pointLightColor * max(dot(normalize(fragNormalWorld), normalize(directionToLight)), 0);

    aColor = vec4((diffuseLight + ambientLight) * fragColor, 1.0);
}