#version 450

layout(location = 0) out vec4 aColor;
layout(location = 0) in vec3 fragColor;

layout(push_constant) uniform Push {
    mat4 transform;
    vec3 color;
} push;

void main() {
    aColor = vec4(fragColor, 1.0);
}