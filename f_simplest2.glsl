#version 330
uniform sampler2D textureMap0;


out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

in vec2 iTexCoord0; //Współrzędne tekstury

void main(void) {


	vec4 texColor = texture(textureMap0, iTexCoord0); //Pobranie koloru z tekstury

	pixelColor= mix(texColor, vec4(0.2,0.3, 0.4, 1),0.3); //Zapisanie koloru do zmiennej wyjściowej
}
