#version 330 core

in vec2 iTexCoord0;
out vec4 pixelColor;

uniform sampler2D textureMap0;

void main() {
    pixelColor = texture(textureMap0, iTexCoord0);
}
