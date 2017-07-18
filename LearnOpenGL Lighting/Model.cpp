#include "Model.h"

void Model::Draw(Shader& shader)
{
	for (GLuint i = 0; i < this->meshes.size(); i++)
	{
		this->meshes[i].Draw(shader);
	}
}

void Model::loadModel(const std::string& path)
{

	m_aiScene = m_importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

	// check if scene or root node are null, or if flagged as incomplete
	if (!m_aiScene || m_aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_aiScene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << m_importer.GetErrorString() << std::endl;
		return;
	}

	std::cout << "Meshes: " << m_aiScene->mNumMeshes << std::endl << "Textures: " << m_aiScene->mNumMaterials << std::endl << "Animations: " << m_aiScene->mNumAnimations << std::endl;

	m_GlobalInverseTransform = m_aiScene->mRootNode->mTransformation;
	m_GlobalInverseTransform.Inverse();

	this->directory = path.substr(0, path.find_last_of('/'));
	// pass root node to recursive processNode function
	this->processNode(m_aiScene->mRootNode, m_aiScene);

	std::cout << "Mesh loaded." << std::endl;
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// Process all the node's meshes (if any)
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}

	// Then do the same for each of its children
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	std::vector<VertexBoneData> bones;

	m_BoneMapping = std::map<std::string, GLuint>();

	std::cout << "Mesh: " <<
		" Vertices: " << mesh->mNumVertices <<
		" Indices: " << mesh->mNumFaces <<
		" Textures: " << mesh->mMaterialIndex <<
		" Bones: " << mesh->mNumBones << std::endl;

	// Process vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// Process vertex positions
		glm::vec3 pos = glm::vec3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
			);
		vertex.Position = pos;
		// Process normals
		glm::vec3 norm = glm::vec3(
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		);
		vertex.Normal = norm;
		// Process texture coords (if any)
		if (mesh->mTextureCoords[0])	// are there texture coordinates?
		{
			glm::vec2 tex = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			);
			vertex.TexCoords = tex;
		}
		else
		{
			glm::vec2 tex = glm::vec2(0.0f);
			vertex.TexCoords = tex;
		}
		// push vertex
		vertices.push_back(vertex);
	}
	// Process indices
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process bones
	//bones.resize(mesh->mNumVertices);
	//for (GLuint i = 0; i < mesh->mNumBones; i++)
	//{
	//	GLuint boneIndex = 0;
	//	std::string boneName(mesh->mBones[i]->mName.data);

	//	if (m_BoneMapping.find(boneName) == m_BoneMapping.end())
	//	{
	//		boneIndex = m_NumBones;
	//		m_NumBones++;
	//		BoneInfo bInfo;
	//		m_BoneInfo.push_back(bInfo);
	//	}
	//	else
	//	{
	//		boneIndex = m_BoneMapping[boneName];
	//	}

	//	m_BoneMapping[boneName] = boneIndex;
	//	m_BoneInfo[boneIndex].boneOffset = AssimpMatToGlmMat(mesh->mBones[i]->mOffsetMatrix);

	//	//for (GLuint j = 0; j < mesh->mBones[i]->mNumWeights; j++)
	//	//{
	//	//	GLuint vertexID = 
	//	//}
	//	
	//}

	// Process material
	if (mesh->mMaterialIndex >= 0)
	{
		// retrieve aiMaterial object from scene mMaterials array
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// get diffuse textures and add them
		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// get specular maps and add them
		std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures, bones);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		GLboolean skip = false;
		for (GLuint j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		// only load if not loaded already
		if (!skip)
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
		}
	}

	return textures;
}


GLint TextureFromFile(const char* path, std::string directory)
{
	// Generate texture ID and load texture data
	std::string filename = std::string(path);
	filename = directory + '/' + filename;
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}