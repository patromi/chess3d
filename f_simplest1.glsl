#version 330
uniform sampler2D textureMap0;


out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

in vec2 iTexCoord0; //Współrzędne tekstury

void main(void) {


	vec4 texColor = texture(textureMap0, iTexCoord0); //Pobranie koloru z tekstury

	pixelColor= texColor; //Zapisanie koloru do zmiennej wyjściowej
}
