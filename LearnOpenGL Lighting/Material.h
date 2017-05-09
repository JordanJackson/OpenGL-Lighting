#pragma once

#include <glm/glm.hpp>

class Material
{
public:
	Material() : ambient(glm::vec3(0.0f)), diffuse(glm::vec3(0.0f)), specular(glm::vec3(0.0f)), shininess(1)
	{

	}
	Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess) : ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess)
	{

	}

private:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
	GLuint diffuseMap;
	GLuint specularMap;
	GLuint emissionMap;

public:
	glm::vec3 Ambient()
	{
		return ambient;
	}

	glm::vec3 Diffuse()
	{
		return diffuse;
	}

	glm::vec3 Specular()
	{
		return specular;
	}

	float Shininess()
	{
		return shininess;
	}

	void InitializeDiffuseMap(unsigned char* image, int width, int height)
	{
		glGenTextures(1, &diffuseMap);

		InitializeMap(diffuseMap, image, width, height);
	}

	void InitializeSpecularMap(unsigned char* image, int width, int height)
	{
		glGenTextures(1, &specularMap);

		InitializeMap(specularMap, image, width, height);
	}

	void InitializeEmissionMap(unsigned char* image, int width, int height)
	{
		glGenTextures(1, &emissionMap);

		InitializeMap(emissionMap, image, width, height);
	}

	void InitializeMap(const GLuint& map, unsigned char* image, int width, int height)
	{
		glBindTexture(GL_TEXTURE_2D, map);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	}

	GLuint DiffuseMap()
	{
		return diffuseMap;
	}

	GLuint SpecularMap() 
	{
		return specularMap;
	}

	GLuint EmissionMap()
	{
		return emissionMap;
	}
};
