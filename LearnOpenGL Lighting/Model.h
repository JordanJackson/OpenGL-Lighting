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

#include "Math_Utils.h"
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
	// structs
	struct BoneInfo
	{
		glm::mat4 boneOffset;
		glm::mat4 finalTransformation;

		BoneInfo()
		{
			boneOffset = glm::mat4();
			finalTransformation = glm::mat4();
		}
	};

private:
	// Model
	const aiScene* m_aiScene;
	Assimp::Importer m_importer;

	aiMatrix4x4 m_GlobalInverseTransform;

	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;	// stores all texture loaded so far, optimization to prevent unnecessary texture loading

	// Bones
	std::map<std::string, GLuint> m_BoneMapping;
	GLuint m_NumBones;
	std::vector<BoneInfo> m_BoneInfo;

private:
	// Functions
	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);


};

