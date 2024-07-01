#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include"Shader.h"

#include<GLFW/glfw3.h>


// #include <vector>
class Camera
{
public:
	// Stores the main vectors of the camera
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;

	// Stores the width and height of the window
	int width;
	int height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed = 10.5f;
	float sensitivity = 100.0f;

	// Camera constructor to set up initial values
	Camera(int width, int height, glm::vec3 position);

	// Updates and exports the camera matrix to the Vertex Shader
	std::tuple<glm::mat4, glm::vec3> Matrix(float FOVdeg, float nearPlane, float farPlane);
	// Handles camera inputs
	void Inputs(GLFWwindow* window);
};