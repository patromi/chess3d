#version 330 core

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec2 texCoord; // zakładamy, że UV są tu

out vec2 iTexCoord0;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main() {
    gl_Position = P * V * M * vertex;
    iTexCoord0 = texCoord;
}
