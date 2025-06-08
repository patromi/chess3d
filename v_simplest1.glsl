#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec4 lp[4];  // tablica 4 świateł

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 normal;
layout(location = 3) in vec2 texCoord0;

out vec2 iTexCoord0; 
out vec4 iC;
out vec4 l;  // pojedynczy wektor światła
out vec4 n;
out vec4 v;

void main(void) {
    // Dla prostoty - weź pierwsze źródło światła
    l = normalize(V * (lp[0] - M * vertex));
    n = normalize(V * M * normal);
    v = normalize(vec4(0, 0, 0, 1) - V * M * vertex);
    iC = color;
    iTexCoord0 = texCoord0;

    gl_Position = P * V * M * vertex;
}
