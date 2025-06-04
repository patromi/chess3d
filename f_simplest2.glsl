#version 330
uniform sampler2D textureMap0;
uniform sampler2D textureMap1;

out vec4 pixelColor;

in vec2 iTexCoord0;

void main(void) {


	vec4 texColor0 = texture(textureMap0, iTexCoord0);
	
	vec4 texColor1 = texture(textureMap1, iTexCoord0);

	pixelColor= mix(texColor0,texColor1, 0.2);
}
