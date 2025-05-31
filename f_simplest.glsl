#version 330

uniform sampler2D textureMap0; // diffuse map
uniform sampler2D textureMap1; // specular map
uniform vec3 pieceColor;       // kolor figury przekazany z CPU

in vec2 iTexCoord0;
in vec4 l; // wektor do światła
in vec4 n; // normalna
in vec4 v; // wektor do kamery

out vec4 pixelColor;

void main(void) {
    // Pobranie kolorów z tekstur
    vec4 texColor = texture(textureMap0, iTexCoord0);
    vec3 baseColor = texColor.rgb;
    vec3 specularMap = texture(textureMap1, iTexCoord0).rgb;
    float alpha = texColor.a;

    // Normalizacja wektorów
    vec3 lightDir = normalize(l.xyz);
    vec3 normal = normalize(n.xyz);
    vec3 viewDir = normalize(v.xyz);
    vec3 reflectDir = reflect(-lightDir, normal);

    // Oświetlenie
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), 80.0);

    // Światło otoczenia
    vec3 ambient = vec3(0.2);

    // Fresnel – rozjaśnienie krawędzi
    float fresnel = pow(1.0 - max(dot(normal, viewDir), 0.0), 2.0);
    vec3 fresnelEffect = vec3(1.0) * fresnel * 0.05;

    // Mieszanie koloru figury z teksturą (subtelne)
    float mixFactor = 0.95; // stała wartość – lepsza kontrola
    vec3 mixedColor = mix(baseColor, pieceColor, mixFactor);

    // Finalny kolor
    vec3 diffuse = diff * mixedColor;
    vec3 specular = specularMap * spec * 1.5;
    vec3 finalColor = ambient * mixedColor + diffuse + specular + fresnelEffect;

    pixelColor = vec4(finalColor, alpha);
}
