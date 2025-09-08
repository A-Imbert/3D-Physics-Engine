#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H
#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shaderClass.h"

class Camera
{
public:
	//Where the camera is located
	glm::vec3 position;
	//Which way the camera is facing
	glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	//Defining "up" which is needed for camera location
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	//SCREEN DIMENSIONS
	int width;
	int height;

	//MOVEMENT CONTROLS
	float speed = 0.1f;
	float sensitivity = 100.0f;

	Camera(int width, int height, glm::vec3 position) {
		Camera::width = width;
		Camera::height = height;
		this->position = position;
	}
	//Creates View and projection matrix and sends them to the shader.
	void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform) {
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		/*
		1. Where the camera is.
		2. Where the camera is looking.
		3. Up direction
		*/
		view = glm::lookAt(position, position + orientation, up);
		//Field of View in Degrees, Aspect Ratio, nearPlane, farPlane
		projection = glm::perspective(glm::radians(FOVdeg), (float)width / (float)height, nearPlane, farPlane);

		glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
	}
	//void Inputs(GLFWwindow* window);
};
#endif

