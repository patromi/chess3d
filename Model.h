#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Mesh.h"
#include "shaderprogram.h"

class Model {
public:
	Model();
	Model(const std::string& path);
	void loadModel(const std::string& path);
	void drawMeshByName(const std::string& name, ShaderProgram* sp) const;

private:
	std::vector<Mesh> meshes;
	std::unordered_map<std::string, size_t> meshNameToIndex;

	void processNode(struct aiNode* node, const struct aiScene* scene);
	Mesh processMesh(struct aiMesh* mesh);
};