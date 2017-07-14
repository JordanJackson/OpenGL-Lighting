#include "Transform.h"



Transform::Transform() 
	: position(glm::vec3(0.0f)), rotation(glm::quat()), scale(glm::vec3(1.0f)), children(std::vector<Transform*>()), parent(nullptr), dirtyFlag(true)
{
	computeMatrix(glm::mat4());
}

Transform::Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale)
	: position(position), rotation(rotation), scale(scale), children(std::vector<Transform*>()), parent(nullptr), dirtyFlag(true)
{
	computeMatrix(glm::mat4());
}

Transform::~Transform()
{
}

void Transform::computeMatrix(const glm::mat4& parentMatrix)
{

	if (dirtyFlag)
	{
		std::cout << "Computing" << std::endl;
		matrix = parentMatrix;
		matrix = glm::translate(matrix, position);
		matrix *= glm::toMat4(rotation);
		matrix = glm::scale(matrix, scale);

		for (int i = 0; i < children.size(); i++)
		{
			children[i]->computeMatrix(matrix);
		}

		dirtyFlag = false;
	}
}

void Transform::setParent(Transform& parentTransform)
{
	parent = &parentTransform;
	parentTransform.addChild(*this);
}

void Transform::addChild(Transform& childTransform)
{
	children.push_back(&childTransform);
	childTransform.dirtyFlag = true;
	childTransform.computeMatrix(matrix);
}

void Transform::removeChild(Transform* childTransform)
{
	children.erase(std::remove(children.begin(), children.end(), childTransform), children.end());
}

glm::mat4 Transform::getMatrix()
{
	return matrix;
}

void Transform::translate(const glm::vec3& move)
{
	position += move;
	dirtyFlag = true;
	dirtyChildren();
}

void Transform::rotate(float angle, const glm::vec3& axis)
{
	rotation *= glm::quat(axis * angle);
	dirtyFlag = true;
	dirtyChildren();
}

void Transform::dirtyChildren()
{
	for (int i = 0; i < children.size(); i++)
	{
		children[i]->dirtyFlag = true;
	}
}

glm::vec3 Transform::forward()
{
	return rotation * glm::vec3(0.0f, 0.0f, 1.0f);
}

glm::vec3 Transform::up()
{
	return rotation * glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 Transform::right()
{
	return rotation * glm::vec3(1.0f, 0.0f, 0.0f);
}