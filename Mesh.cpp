#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::string& name)
{
	this->name = name;
	this->vertexCount = vertices.size();
	this->verticesData = new float[vertexCount * 4];
	for (size_t i = 0; i < vertexCount; ++i) {
		this->verticesData[i * 4] = vertices[i].position.x;
		this->verticesData[i * 4 + 1] = vertices[i].position.y;
		this->verticesData[i * 4 + 2] = vertices[i].position.z;
		this->verticesData[i * 4 + 3] = vertices[i].position.w;
	}

	this->normalsData = new float[vertexCount * 4];
	for (size_t i = 0; i < vertexCount; ++i) {
		this->normalsData[i * 4] = vertices[i].normal.x;
		this->normalsData[i * 4 + 1] = vertices[i].normal.y;
		this->normalsData[i * 4 + 2] = vertices[i].normal.z;
		this->normalsData[i * 4 + 3] = vertices[i].normal.w;
	}

	this->indexesCount = indices.size();
	this->indexesData = new unsigned int[indexesCount];
	for (size_t i = 0; i < indexesCount; ++i) {
		this->indexesData[i] = indices[i];
	}

}

void Mesh::draw(ShaderProgram* sp) const {

	glEnableVertexAttribArray(sp->a("vertex"));  //W³¹cz przesy³anie danych do atrybutu vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, this->verticesData);

	glEnableVertexAttribArray(sp->a("normal"));  //W³¹cz przesy³anie danych do atrybutu normal
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, this->normalsData);

	glDrawElements(GL_TRIANGLES, this->indexesCount, GL_UNSIGNED_INT, this->indexesData);
	//glDrawArrays(GL_TRIANGLES, 0, this->vertexCount); //Narysuj obiekt

	glDisableVertexAttribArray(sp->a("vertex"));  //Wy³¹cz przesy³anie danych do atrybutu vertex
	glDisableVertexAttribArray(sp->a("normal"));  //Wy³¹cz przesy³anie danych do atrybutu normal

}

const std::string& Mesh::getName() const {
	return name;
}
