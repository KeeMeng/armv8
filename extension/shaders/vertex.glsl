#version 330 core

// take input from vertex buffer object data
layout (location = 0) in vec2 vertexPos;
layout (location = 1) in vec2 atexCoords;

out vec2 TexCoords;

uniform mat4 project;
uniform mat4 model;
uniform mat3 texCoordsTransform;

void main() {
    vec3 temp = vec3(atexCoords.x, 1.0 - atexCoords.y, 1.0);
    temp = texCoordsTransform * temp;
    TexCoords = temp.xy;

    // set gl_Position global variable to inputted position
    gl_Position = project * model * vec4(vertexPos.x, vertexPos.y, 0.0, 1.0);
};