#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <iostream>
#include <vector>

class Transform
{

private:
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	glm::mat4 matrix;

	std::vector<Transform*> children;

	Transform* parent;

	bool dirtyFlag;

public:
	Transform();
	Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale);
	~Transform();

	void computeMatrix(const glm::mat4& parentMatrix);
	void setParent(Transform& parentTransform);
	void addChild(Transform& childTransform);
	void removeChild(Transform* childTransform);
	void dirtyChildren();

	void translate(const glm::vec3& move);
	void rotate(float angle, const glm::vec3& axis);

	glm::vec3 forward();
	glm::vec3 up();
	glm::vec3 right();

	glm::mat4 getMatrix();
};

