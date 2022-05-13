#pragma once
#include"Shader.h"

#include<GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include <bits/stdc++.h>
class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Orientation;
	glm::vec3 Up;

	const float width;
	const float height;

	Camera(int width, int height, const glm::vec3 position, const glm::vec3 Orientation, const glm::vec3 Up);	


	const std::tuple<glm::mat4, glm::vec4, glm::vec4, glm::vec4, glm::vec4> Matrix(float FOVdeg, float nearPlane, float farPlane);

};
