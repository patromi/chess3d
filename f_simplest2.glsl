#version 330
uniform sampler2D textureMap0;
uniform sampler2D textureMap1;

uniform bool isMoved;
out vec4 pixelColor;
in vec4 iC;

in vec4 l;
in vec4 n;
in vec4 v;

in vec2 iTexCoord0;

void main(void) {

	vec4 ml = normalize(l);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);
	vec4 mr = reflect(-ml, mn);

	vec4 texColor0 = texture(textureMap0, iTexCoord0);
	vec4 texColor1 = texture(textureMap1, iTexCoord0);
	vec4 texColor;

	if (isMoved) {
		texColor = mix(mix(texColor0, texColor1, 0.2),vec4(0.2, 0.5, 0.2, 1.0),0.8);
	} else {
		texColor = mix(texColor0, texColor1, 0.2);
	
	}

	//vec4 texColor = mix(texColor0, texColor1, 0.2);

	float nl = clamp(dot(mn, ml), 0, 1);
	float rv = pow(clamp(dot(mr, mv), 0, 1), 25);

	pixelColor = vec4(nl * texColor.rgb, texColor.a) + vec4(rv, rv, rv, 0);
}
