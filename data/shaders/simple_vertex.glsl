#version 330 core

uniform mat4 modelViewProjection;
uniform mat4 localToWorld;
// TODO: add camera position

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 texCoords;

out Data {
    vec3 normal;
    vec3 position;
    vec2 texCoords;
} outData;

void main() {
    gl_Position = modelViewProjection * vec4(position, 1);
    outData.normal = mat3x3(localToWorld) * normal;
    outData.position = mat3x3(localToWorld) * position;
    outData.texCoords = texCoords;
}
