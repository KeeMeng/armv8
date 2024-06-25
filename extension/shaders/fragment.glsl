#version 330 core

in vec2 TexCoords;
out vec4 colour;

uniform sampler2D image;
uniform vec3 spriteColour;
uniform bool useTexture;

void main() {
    if (useTexture) {
        colour = texture(image, TexCoords) * vec4(spriteColour, 1.0);
    } else {
        colour = vec4(spriteColour, 1.0);
    }
};