#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec4 lp[2];

in vec4 vertex;
in vec4 color;
in vec4 normal;

in vec2 texCoord0;

out vec2 iTexCoord0; 
out vec4 iC;
out vec4 l[2];
out vec4 n;
out vec4 v;

void main(void) {
    for (int i = 0; i < 2; i++) {
        l[i] = normalize(V * (lp[i] - M * vertex));
    }
    n = normalize(V * M * normal);
    v = normalize(vec4(0, 0, 0, 1) - V * M * vertex);
    iC = color;
    iTexCoord0 = texCoord0;

    gl_Position = P * V * M * vertex;
}
