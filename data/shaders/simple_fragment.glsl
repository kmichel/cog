#version 330 core

uniform sampler2D albedo;
uniform samplerCube environment;

in Data {
    vec3 normal;
    vec3 position;
    vec2 texCoords;
} inData;

out vec4 color;

void main() {
    //color = vec4(0.5, 0, 0, 0.5);
    //color = vec4(normalize(inData.normal), 1);
    color = texture(albedo, inData.texCoords) * 0.01f;
    color += texture(environment, normalize(inData.normal));
}
