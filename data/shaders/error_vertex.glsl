#version 330 core

uniform mat4 modelViewProjection;

layout (location=0) in vec3 position;

void main() {
    gl_Position = modelViewProjection * vec4(position, 1);
}
