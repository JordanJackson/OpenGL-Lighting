#pragma once

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 3.0f;
const GLfloat SENSITIVITY = 0.25f;
const GLfloat ZOOM = 45.0f;

class Camera
{

private:
	// Camera attributes
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	// Euler angles
	GLfloat yaw;
	GLfloat pitch;
	// Camera options
	GLfloat movementSpeed;
	GLfloat mouseSensitivity;
	GLfloat zoom;

public:
	// Vector constructor
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) 
		: position(position), worldUp(up), yaw(yaw), pitch(pitch), front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
	{
		this->updateCameraVectors();
	}

	// Scalar value constructor
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch)
		: position(glm::vec3(posX, posY, posZ)), worldUp(glm::vec3(upX, upY, upZ)), yaw(yaw), pitch(pitch), front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
	{
		this->updateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->position, this->position + this->front, this->up);
	}

	// Processes input received from any keyboard-like input system.
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->movementSpeed * deltaTime;
		if (direction == FORWARD)
		{
			this->position += this->front * velocity;
		}
		if (direction == BACKWARD)
		{
			this->position -= this->front * velocity;
		}
		if (direction == LEFT)
		{
			this->position -= this->right * velocity;
		}
		if (direction == RIGHT)
		{
			this->position += this->right * velocity;
		}
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= this->mouseSensitivity;
		yoffset *= this->mouseSensitivity;

		this->yaw += xoffset;
		this->pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (this->pitch > 89.0f)
			{
				this->pitch = 89.0f;
			}
			if (this->pitch < -89.0f)
			{
				this->pitch = -89.0f;
			}
		}

		// Update front, right and up vectors using the updated Euler angles
		this->updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. only requires input on the vertical wheel-axis
	void ProcessMouseScroll(GLfloat yoffset)
	{
		if (this->zoom >= 1.0f && this->zoom <= 45.0f)
		{
			this->zoom -= yoffset;
		}
		if (this->zoom <= 1.0f)
		{
			this->zoom = 1.0f;
		}
		if (this->zoom >= 45.0f)
		{
			this->zoom = 45.0f;
		}
	}

	GLfloat Zoom()
	{
		return zoom;
	}

	glm::vec3 Position()
	{
		return position;
	}

private:

	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 f;

		f.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		f.y = sin(glm::radians(this->pitch));
		f.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front = glm::normalize(f);
		// Also recalculate the Right and Up vector
		this->right = glm::normalize(glm::cross(this->front, this->worldUp));
		this->up = glm::normalize(glm::cross(this->right, this->front));
	}
};
