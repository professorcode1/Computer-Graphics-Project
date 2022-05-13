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



Camera::Camera(int width, int height, const glm::vec3 position, const glm::vec3 Orientation,  glm::vec3 Up) : width(width), height(height) {	
	this->Position = position;
	this->Orientation = glm::normalize(Orientation);
	this->Up = glm::normalize(Up);
}

const std::tuple<glm::mat4, glm::vec4, glm::vec4, glm::vec4, glm::vec4> Camera::Matrix(float FOVdeg, float nearPlane, float farPlane){
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	view = glm::lookAt(Position, Position + Orientation, Up);
	projection = glm::perspective(glm::radians(FOVdeg), width / height, nearPlane, farPlane);
	using std::cout, std::endl;
	using glm::to_string, glm::mat4, glm::vec4,glm::vec3;
	mat4 matrix_transform   = projection * view, matrix_transform_inv = glm::inverse(projection * view);
	vec4 up_left_front   	= matrix_transform_inv * vec4(-1.0f, 1.0f,  1.0f, 1.0f);
	vec4 up_left_back   	= matrix_transform_inv * vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	vec4 up_right_front 	= matrix_transform_inv * vec4( 1.0f, 1.0f,  1.0f, 1.0f);
	vec4 up_right_back 		= matrix_transform_inv * vec4( 1.0f, 1.0f, -1.0f, 1.0f);
	vec4 down_left_front 	= matrix_transform_inv * vec4(-1.0f,-1.0f,  1.0f, 1.0f);
	vec4 down_left_back 	= matrix_transform_inv * vec4(-1.0f,-1.0f, -1.0f, 1.0f);
	vec4 down_right_front 	= matrix_transform_inv * vec4( 1.0f,-1.0f,  1.0f, 1.0f);
	vec4 down_right_back 	= matrix_transform_inv * vec4( 1.0f,-1.0f, -1.0f, 1.0f);


	up_left_front /= up_left_front.w;
	up_left_back /= up_left_back.w;
	up_right_front /= up_right_front.w;
	up_right_back /= up_right_back.w;
	down_left_front /= down_left_front.w;
	down_left_back /= down_left_back.w;
	down_right_front /= down_right_front.w;
	down_right_back /= down_right_back.w;

	{    
		std::ofstream file("viewing_Frustom.obj");
		file<<"v "<<up_left_front.x<< " " <<up_left_front.y<< " " <<up_left_front.z<<'\n';
		file<<"v "<<up_left_back.x<< " " <<up_left_back.y<< " " <<up_left_back.z<<'\n';
		file<<"v "<<up_right_front.x<< " " <<up_right_front.y<< " " <<up_right_front.z<<'\n';
		file<<"v "<<up_right_back.x<< " " <<up_right_back.y<< " " <<up_right_back.z<<'\n';
		file<<"v "<<down_left_front.x<< " " <<down_left_front.y<< " " <<down_left_front.z<<'\n';
		file<<"v "<<down_left_back.x<< " " <<down_left_back.y<< " " <<down_left_back.z<<'\n';
		file<<"v "<<down_right_front.x<< " " <<down_right_front.y<< " " <<down_right_front.z<<'\n';
		file<<"v "<<down_right_back.x<< " " <<down_right_back.y<< " " <<down_right_back.z<<'\n';
		file<<"f "<<"1 2 4 \n";
		file<<"f "<<"1 4 3 \n";
		file<<"f "<<"1 3 5 \n";
		file<<"f "<<"3 7 5 \n";
		file<<"f "<<"3 4 7 \n";
		file<<"f "<<"7 4 8 \n";
		file<<"f "<<"5 7 8 \n";
		file<<"f "<<"5 8 6 \n";
		file<<"f "<<"1 5 6 \n";
		file<<"f "<<"1 6 2 \n";
		file<<"f "<<"6 2 4 \n";
		file<<"f "<<"1 8 4 \n";
		file.close();
	}
	cout<<"up_left_front \t"<<to_string(up_left_front)<<endl;
	cout<<"up_left_back \t"<<to_string(up_left_back)<<endl;
	cout<<"up_right_front \t"<<to_string(up_right_front)<<endl;
	cout<<"up_right_back \t"<<to_string(up_right_back)<<endl;
	cout<<"down_left_front \t"<<to_string(down_left_front)<<endl;
	cout<<"down_left_back \t"<<to_string(down_left_back)<<endl;
	cout<<"down_right_front \t"<<to_string(down_right_front)<<endl;
	cout<<"down_right_back \t"<<to_string(down_right_back)<<endl;

	float up_left_lambda = up_left_front.y / (up_left_front.y - up_left_back.y); 
	float up_right_lambda = up_right_front.y / (up_right_front.y - up_right_back.y); 
	float down_left_lambda = down_left_front.y / (down_left_front.y - down_left_back.y); 
	float down_right_lambda = down_right_front.y / (down_right_front.y - down_right_back.y); 

	// cout<<up_left_lambda<<endl;
	// cout<<up_right_lambda<<endl;
	// cout<<down_left_lambda<<endl;
	// cout<<down_right_lambda<<endl;

	int number_of_intersection = up_left_lambda
								 + up_right_lambda
								 + down_left_lambda
								 + down_right_lambda;



	vec4 up_left_intr = up_left_front + up_left_lambda * ( up_left_back - up_left_front ) ;
	vec4 up_right_intr = up_right_front + up_right_lambda * ( up_right_back - up_right_front ) ;
	vec4 down_left_intr = down_left_front + down_left_lambda * ( down_left_back - down_left_front ) ;
	vec4 down_right_intr = down_right_front + down_right_lambda * ( down_right_back - down_right_front ) ;

	{
	    std::ofstream file_("viewing_Frustom_Clipped.obj");
		file_<<"v "<<up_left_front.x<< " " <<up_left_front.y<< " " <<up_left_front.z<<'\n';
		file_<<"v "<<up_left_back.x<< " " <<up_left_back.y<< " " <<up_left_back.z<<'\n';
		file_<<"v "<<up_right_front.x<< " " <<up_right_front.y<< " " <<up_right_front.z<<'\n';
		file_<<"v "<<up_right_back.x<< " " <<up_right_back.y<< " " <<up_right_back.z<<'\n';
		file_<<"v "<<down_left_front.x<< " " <<down_left_front.y<< " " <<down_left_front.z<<'\n';
		file_<<"v "<<down_left_back.x<< " " <<down_left_back.y<< " " <<down_left_back.z<<'\n';
		file_<<"v "<<down_right_front.x<< " " <<down_right_front.y<< " " <<down_right_front.z<<'\n';
		file_<<"v "<<down_right_back.x<< " " <<down_right_back.y<< " " <<down_right_back.z<<'\n';
		file_<<"v "<<up_left_intr.x<<" "<<up_left_intr.y<<" "<<up_left_intr.z<<'\n';
		file_<<"v "<<up_right_intr.x<<" "<<up_right_intr.y<<" "<<up_right_intr.z<<'\n';
		file_<<"v "<<down_left_intr.x<<" "<<down_left_intr.y<<" "<<down_left_intr.z<<'\n';
		file_<<"v "<<down_right_intr.x<<" "<<down_right_intr.y<<" "<<down_right_intr.z<<'\n';
		// file_<<"f "<<"1 2 4 \n";
		// file_<<"f "<<"1 4 3 \n";
		// file_<<"f "<<"1 3 5 \n";
		// file_<<"f "<<"3 7 5 \n";
		// file_<<"f "<<"3 4 7 \n";
		// file_<<"f "<<"7 4 8 \n";
		// file_<<"f "<<"5 7 8 \n";
		// file_<<"f "<<"5 8 6 \n";
		// file_<<"f "<<"1 5 6 \n";
		// file_<<"f "<<"1 6 2 \n";
		// file_<<"f "<<"6 2 4 \n";
		// file_<<"f "<<"1 8 4 \n";
		file_<<"f "<<"9 10 2 \n";
		file_<<"f "<<"10 11 2 \n";
		file_<<"f "<<"11 12 2 \n";
		file_<<"f "<<"12 9 2 \n";

		file_<<"f "<<"9 10  8\n";
		file_<<"f "<<"10 11 8 \n";
		file_<<"f "<<"11 12 8 \n";
		file_<<"f "<<"12 9  8\n";

		file_<<"f "<<"9 10  6\n";
		file_<<"f "<<"10 11 6 \n";
		file_<<"f "<<"11 12 6 \n";
		file_<<"f "<<"12 9  6\n";

		file_<<"f "<<"9 10  4\n";
		file_<<"f "<<"10 11 4 \n";
		file_<<"f "<<"11 12 4 \n";
		file_<<"f "<<"12 9  4\n";

		file_.close();
	}

	// cout<<"up_left_intr" << to_string(up_left_intr)<<endl;
	// cout<<"up_right_intr" << to_string(up_right_intr)<<endl;
	// cout<<"down_left_intr" << to_string(down_left_intr)<<endl;
	// cout<<"down_right_intr" << to_string(down_right_intr)<<endl;
	{	
		float mRatio = width / height;
		float nearHeight = 2 * tan(glm::radians(FOVdeg) / 2) * nearPlane;
		float nearWidth = nearHeight * mRatio;

		float farHeight = 2 * tan(glm::radians(FOVdeg) / 2) * farPlane;
		float farWidth = farHeight * mRatio;

		glm::vec3 fc = Position + Orientation * farPlane;
		glm::vec3 nc = Position + Orientation * nearPlane;

		glm::vec3 frustumCorners[8];
		glm::vec3 mRight = glm::normalize(glm::cross(Orientation, Up));
		glm::vec3 mUp = glm::normalize(glm::cross(mRight, Orientation));
		frustumCorners[0] = fc + ( mUp * (farHeight / 2.0f) ) - (mRight * (farWidth / 2.0f));
		frustumCorners[1] = fc + ( mUp * (farHeight / 2.0f) ) + (mRight * (farWidth / 2.0f));
		frustumCorners[2] = fc - ( mUp * (farHeight / 2.0f) ) - (mRight * (farWidth / 2.0f));
		frustumCorners[3] = fc - ( mUp * (farHeight / 2.0f) ) + (mRight * (farWidth / 2.0f));
		frustumCorners[4] = nc + ( mUp * (nearHeight / 2.0f) ) - (mRight *( nearWidth / 2.0f));
		frustumCorners[5] = nc + ( mUp * (nearHeight / 2.0f) ) + (mRight *( nearWidth / 2.0f));
		frustumCorners[6] = nc - ( mUp * (nearHeight / 2.0f) ) - (mRight *( nearWidth / 2.0f));
		frustumCorners[7] = nc - ( mUp * (nearHeight / 2.0f) ) + (mRight *( nearWidth / 2.0f));

		cout<<to_string(frustumCorners[0])<<endl;
		cout<<to_string(frustumCorners[1])<<endl;
		cout<<to_string(frustumCorners[2])<<endl;
		cout<<to_string(frustumCorners[3])<<endl;
		cout<<to_string(frustumCorners[4])<<endl;
		cout<<to_string(frustumCorners[5])<<endl;
		cout<<to_string(frustumCorners[6])<<endl;
		cout<<to_string(frustumCorners[7])<<endl;

	}
	return {matrix_transform, up_left_intr, up_right_intr, down_left_intr, down_right_intr};

}