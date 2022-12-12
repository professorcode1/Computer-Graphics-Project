#include "plane.h"
#include "waveFrontFileApi.h"



std::tuple<glm::mat4,glm::vec3> Plane::get_MVP_Matrix(float FOVdeg, float nearPlane, float farPlane, float aspect){
	glm::vec3 orientation( cos(glm::radians(pitch_degree)) * sin(glm::radians(yay_degree)), 
	sin(glm::radians(pitch_degree)) ,
	 cos(glm::radians(pitch_degree)) * cos(glm::radians(yay_degree)));
	// std::cout<<"Orientation ::\t"<< glm::to_string(orientation)<<std::endl ;
	glm::vec3 view_position = camera_ViewPoint_distance * orientation + position;
	glm::vec3 camera_pos = position +  camera_behind_distant * orientation * -1.0f + 
		glm::normalize(glm::cross(glm::cross(orientation, Up), orientation)) * camera_up_distance;
	glm::vec3 camera_dir_vec = glm::normalize(view_position - camera_pos);
	// Initializes matrices since otherwise they will be the null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(camera_pos, camera_pos + glm::normalize(camera_dir_vec), Up);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(FOVdeg), aspect, nearPlane, farPlane);

	// Exports the camera matrix to the Vertex Shader
	glm::mat4 VP = projection * view;
	// printf("Camera %s \n", glm::to_string(camera_pos).c_str());
	// printf("Plane %s \n", glm::to_string(position).c_str());
	return {VP, camera_pos};	
}

void Plane::render(glm::mat4 viewAndProjection){
    vao.Bind();
    ibo->Bind();
    shader.Bind();
    shader.SetUniform1i(this->texture_UniformName, texture_BindSlot);
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), this->position), glm::vec3(scaling_factor, scaling_factor, scaling_factor));
	model = glm::rotate(model, glm::radians(yay_degree  ), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(pitch_degree), glm::vec3(-1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(roll_degree  ), glm::vec3(0.0f, 0.0f, 1.0f));
    shader.SetUniformMat4f(this->MVP_uniform_name, viewAndProjection * model);
    GLCall(glDrawElements(GL_TRIANGLES, ibo->GetCount() , GL_UNSIGNED_INT, nullptr));
}

void Plane::catchInputs(GLFWwindow* window){
    glm::vec3 orientation( cos(glm::radians(pitch_degree)) * sin(glm::radians(yay_degree)), 
	sin(glm::radians(pitch_degree)) ,
	 cos(glm::radians(pitch_degree)) * cos(glm::radians(yay_degree)));
	bool roll_was_changed = false;
	if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
		pitch_degree = std::min(pitch_degree+rotation_angle_per_frame_deg, 60.0f);

    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
		pitch_degree = std::max(pitch_degree - rotation_angle_per_frame_deg, -60.0f);
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		roll_was_changed = true;
		roll_degree -= rotation_angle_per_frame_deg; 
		yay_degree += rotation_angle_per_frame_deg;
    }
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
		roll_was_changed = true;
		roll_degree += rotation_angle_per_frame_deg; 
		yay_degree -= rotation_angle_per_frame_deg;
    }
    position += speed * orientation;
	if(! roll_was_changed && roll_was_changed > 360.0f){
		roll_degree = glm::mod(roll_degree, 360.0f);
	}
	if(! roll_was_changed  && roll_degree >= 1.0f){
			roll_degree -= rotation_angle_per_frame_deg;
	}

	if(! roll_was_changed  && roll_degree <= -1.0f){
			roll_degree += rotation_angle_per_frame_deg;
	}
	// printf("%s \n", glm::to_string(position).c_str());
}

Plane::Plane(const std::string& computeFile, const std::string& vertexFile, const std::string& fragFile, const std::string& modelObjFile,
	const std::string &texFile, int binding_Pnt,float camera_behind_distant,float camera_up_distance, float camera_ViewPoint_distance,float scaling_factor,float speed
	, const VertexBufferLayout &vertex_layout, const std::string MVP_uniform_name, const std::string &texture_UniformName):
vbo(nullptr), ibo(nullptr), collition_detection(computeFile), shader(vertexFile,fragFile),
camera_behind_distant(camera_behind_distant) ,camera_up_distance( camera_up_distance),camera_ViewPoint_distance(camera_ViewPoint_distance),  tex(texFile),
MVP_uniform_name(MVP_uniform_name),  texture_UniformName(texture_UniformName),scaling_factor(scaling_factor), speed(speed) {
	std::vector<vertex_t> vertices_plane;
	std::vector<unsigned int> index_buffer_plane; 
	parse_simple_wavefront(modelObjFile, vertices_plane, index_buffer_plane);
	vbo = new VertexBuffer(vertices_plane.data(), vertices_plane.size() * sizeof(vertex_t));
	ibo = new IndexBuffer(index_buffer_plane.data(), index_buffer_plane.size());
	vao.AddBuffer(*vbo, vertex_layout);
	shader.Bind();
	texture_BindSlot = binding_Pnt; 
	tex.Bind(binding_Pnt);
	shader.SetUniform1i(texture_UniformName, binding_Pnt);
}