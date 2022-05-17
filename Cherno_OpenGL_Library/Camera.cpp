#include"Camera.h"
#include "Renderer.h"
#include <cassert>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/trigonometric.hpp>
#include <math.h>
#include <numeric>



Camera::Camera(int width, int height, const glm::vec3 position, const glm::vec3 Orientation,  glm::vec3 Up) : width(width), height(height) {	
	this->Position = position;
	this->Orientation = glm::normalize(Orientation);
	this->Up = glm::normalize(Up);
}

const std::tuple<glm::mat4, float,  float,  float,  float> Camera::Matrix(float FOVdeg, float nearPlane, float farPlane
,bool write_frustum_to_file, float padding,const char* file_name){
	using std::cout, std::endl;
	using glm::to_string, glm::mat4, glm::vec4,glm::vec3;

	float mRatio = width / height;
	glm::mat4 view = glm::lookAt(Position, Position + Orientation, Up);
	glm::mat4 projection = glm::perspective(glm::radians(FOVdeg), width / height, nearPlane, farPlane);
	float nearHeight = 2 * tan(glm::radians(FOVdeg) / 2) * nearPlane;
	float nearWidth = nearHeight * mRatio;
	float farHeight = 2 * tan(glm::radians(FOVdeg) / 2) * farPlane;
	float farWidth = farHeight * mRatio;
	glm::vec3 fc = Position + Orientation * farPlane;
	glm::vec3 nc = Position + Orientation * nearPlane;
	glm::vec3 frustumCorners[8];
	glm::vec3 mRight = glm::normalize(glm::cross(Orientation, Up));
	glm::vec3 mUp = glm::normalize(glm::cross(mRight, Orientation));
	frustumCorners[0] = fc + ( mUp * (farHeight / 2.0f) )  - (mRight * (farWidth / 2.0f));   //up_left_front
	frustumCorners[1] = fc + ( mUp * (farHeight / 2.0f) )  + (mRight * (farWidth / 2.0f));	//up_right_front
	frustumCorners[2] = fc - ( mUp * (farHeight / 2.0f) )  - (mRight * (farWidth / 2.0f));	//down_left_front
	frustumCorners[3] = fc - ( mUp * (farHeight / 2.0f) )  + (mRight * (farWidth / 2.0f));	//down_right_front
	frustumCorners[4] = nc + ( mUp * (nearHeight / 2.0f) ) - (mRight *( nearWidth / 2.0f)); //up_left_back
	frustumCorners[5] = nc + ( mUp * (nearHeight / 2.0f) ) + (mRight *( nearWidth / 2.0f)); //up_right_back
	frustumCorners[6] = nc - ( mUp * (nearHeight / 2.0f) ) - (mRight *( nearWidth / 2.0f)); //down_left_back
	frustumCorners[7] = nc - ( mUp * (nearHeight / 2.0f) ) + (mRight *( nearWidth / 2.0f)); //down_right_back
	float min_x, max_x, min_z, max_z;
	min_x = frustumCorners[0].x;
	max_x = frustumCorners[0].x;
	min_z = frustumCorners[0].z;
	max_z = frustumCorners[0].z;
	for(int iter_i = 0 ; iter_i < 8 ; iter_i ++){
		min_x = std::min(min_x, frustumCorners[iter_i].x);
		max_x = std::max(max_x, frustumCorners[iter_i].x);	
		min_z = std::min(min_z, frustumCorners[iter_i].z);	
		max_z = std::max(max_z, frustumCorners[iter_i].z);
	}
	min_x += -1 * padding;
	max_x +=  1 * padding;
	min_z += -1 * padding;
	max_z +=  1 * padding;
	return {projection * view, min_x, max_x, min_z, max_z};
}