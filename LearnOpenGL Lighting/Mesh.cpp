#include "Mesh.h"


Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures, std::vector<VertexBoneData>& bones)
	: vertices(vertices), indices(indices), textures(textures), bones(bones)
{
	this->setupMesh();
}

void Mesh::Draw(Shader& shader)
{
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;

	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		std::string name = this->textures[i].type;
		if (name == "texture_diffuse")
		{
			name += std::to_string(diffuseNr++);
		}
		else if (name == "texture_specular")
		{
			name += std::to_string(specularNr++);
		}

		glUniform1f(glGetUniformLocation(shader.Program, ("material." + name).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}
	
	glActiveTexture(GL_TEXTURE0);

	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

	//glBindBuffer(GL_ARRAY_BUFFER, this->BoneVB);
	//glBufferData(GL_ARRAY_BUFFER, this->bones.size() * sizeof(VertexBoneData), &this->bones[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

	// Vertex Bone Data
	// Bone ID
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	// Bone Weight
	glEnableVertexAttribArray(4);
	glVertexAttribIPointer(4, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)16);

	glBindVertexArray(0);
}