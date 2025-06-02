#pragma once
#include <glm/glm.hpp>

struct Vertex {
	glm::vec4 position; // Wsp�rz�dne w przestrzeni modelu
	glm::vec4 normal; // Wektor normalny
	glm::vec2 texCoords; // Wsp�rz�dne tekstury
};