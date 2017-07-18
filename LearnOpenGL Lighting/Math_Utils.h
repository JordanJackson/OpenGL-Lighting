#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/matrix4x4.h>

static glm::mat4 AssimpMatToGlmMat(const aiMatrix4x4& matrix)
{
	float values[16] = {
		matrix.a1, matrix.a2, matrix.a3, matrix.a4,
		matrix.b1, matrix.b2, matrix.b3, matrix.b4,
		matrix.c1, matrix.c2, matrix.c3, matrix.c4,
		matrix.d1, matrix.d2, matrix.d3, matrix.d4,
	};

	glm::mat4 newMat = glm::make_mat4(values);
	return newMat;
}