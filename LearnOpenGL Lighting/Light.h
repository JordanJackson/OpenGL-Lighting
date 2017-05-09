#pragma once

#include <glm/glm.hpp>

class Light
{
public:
	Light(glm::vec3 color, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : position(position), ambient(ambient), diffuse(diffuse), specular(specular)
	{
		UpdateColors();
	};

private:
	glm::vec3 color;
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	glm::vec3 diffuseColor;
	glm::vec3 ambientColor;

public:
	glm::vec3 Position() const
	{
		return position;
	}

	glm::vec3 Ambient() const
	{
		return ambient;
	}

	glm::vec3 Diffuse() const
	{
		return diffuse;
	}

	glm::vec3 DiffuseColor() const
	{
		return diffuseColor;
	}

	glm::vec3 AmbientColor() const
	{
		return ambientColor;
	}

	glm::vec3 Specular() const
	{
		return specular;
	}

	glm::vec3 Color() const
	{
		return color;
	}

	void SetColor(glm::vec3 newColor)
	{
		color = newColor;
		UpdateColors();
	}

private:
	void UpdateColors()
	{
		diffuseColor = diffuse * color;
		ambientColor = diffuse * color;
	}
};
