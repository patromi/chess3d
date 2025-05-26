#pragma once
#include <vector>
#include <string>
#include <GL/glew.h>
#include "Vertex.h"
#include "shaderprogram.h"
#include <iostream>

class Mesh {
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::string& name);
	void draw(ShaderProgram* sp) const;
	const std::string& getName() const;

private:

	std::string name;
	int vertexCount = 0; // Number of vertices
	float* verticesData = nullptr; // Pointer to vertex data, if needed
	float* normalsData = nullptr; // Pointer to normal data, if needed

	int indexesCount = 0; // Number of indices
	unsigned int* indexesData = nullptr; // Pointer to index data, if needed
};