#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

Model::Model() {}

Model::Model(const std::string& path) {
	loadModel(path);


}


void Model::drawMeshByName(const std::string& name, ShaderProgram* sp) const {
	auto it = meshNameToIndex.find(name);
	if (it != meshNameToIndex.end()) {
		meshes[it->second].draw(sp);
	}
	else {
		std::cerr << "Mesh '" << name << "' not found\n";
	}
}

void Model::loadModel(const std::string& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices);

	if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		std::cerr << "ASSIMP ERROR: " << importer.GetErrorString() << std::endl;
		return;
	}

	processNode(scene->mRootNode, scene);

	if (meshes.empty()) {
		std::cerr << "No meshes loaded from model: " << path << std::endl;
	}
	else {
		std::cout << "Loaded " << meshes.size() << " meshes from model: " << path << std::endl;
		for (size_t i = 0; i < meshes.size(); ++i) {
			meshNameToIndex[meshes[i].getName()] = i;
			std::cout << "Mesh " << i << ": " << meshes[i].getName() << std::endl;
		}
	}
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh processed = processMesh(mesh);
		meshNameToIndex[processed.getName()] = meshes.size();
		meshes.push_back(processed);
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;
		vertex.position = {
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z,
			1.0f
		};

		if (mesh->HasTextureCoords(0)) {
			vertex.texCoords = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			);
		}
		else {
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		}

		vertex.normal = mesh->HasNormals() ?
			glm::vec4(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, 0.0f) :
			glm::vec4(0.0f);

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}

	std::string meshName = mesh->mName.C_Str();
	if (meshName.empty()) meshName = "mesh_" + std::to_string(rand());

	return Mesh(vertices, indices, meshName);
}