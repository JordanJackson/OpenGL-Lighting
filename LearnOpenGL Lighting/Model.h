#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>>
#include <map>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <SOIL.h>

#include "Mesh.h"
#include "Shader.h"

GLint TextureFromFile(const char* path, std::string directory);

class Model
{
public:
	Model(GLchar* path)
	{
		this->loadModel(path);
	}

	void Draw(Shader& shader);
private:
	// Model
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;	// stores all texture loaded so far, optimization to prevent unnecessary texture loading
	// Functions
	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

