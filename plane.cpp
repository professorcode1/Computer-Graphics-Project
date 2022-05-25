#include "plane.h"
#include "Cherno_OpenGL_Library/VertexArray.h"
#include "Cherno_OpenGL_Library/VertexBufferLayout.h"
#include <cstdio>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/trigonometric.hpp>

void string_split( std::string &line, std::vector<std::string> &split, std::string delimiter ){
	size_t pos = 0;
    while ((pos = line.find(delimiter)) != std::string::npos) {
        split.push_back(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());
    }
}

void parse_simple_wavefront(const std::string& filename, std::vector<vertex_t> &vertices, std::vector< unsigned int> &index_buffer){
	std::vector<float> vertex_buffer;
	std::vector<float> texture_buffer;
	std::vector<float> normal_buffer;
	std::string line;
	std::ifstream file(filename);
	std::vector<std::string> split;
	unsigned int index_count = 0;
	while (getline (file, line)) {
		// std::cout<<line<<std::endl;
		split.clear();
		if(line.size() < 2)
			continue;
		string_split(line, split," ");
		if(split[0] == "v"){
			split.push_back(line);
			vertex_buffer.push_back(std::stof(split[1]));
			vertex_buffer.push_back(std::stof(split[2]));
			vertex_buffer.push_back(std::stof(split[3]));
		}else if(split[0] == "vn"){
			split.push_back(line);
			normal_buffer.push_back(std::stof(split[1]));
			normal_buffer.push_back(std::stof(split[2]));
			normal_buffer.push_back(std::stof(split[3]));
		}else if(split[0] == "vt"){
			split.push_back(line);
			texture_buffer.push_back(std::stof(split[1]));
			texture_buffer.push_back(std::stof(split[2]));
		}else if(split[0] == "f"){
			split.push_back(line);
			std::vector<std::string> split_split;
			vertex_t hlpr_ver;
			// std::cout<<"line\t"<<line<<std::endl;
			// std::cout<<split[1]<<'\t' <<split[2]<<'\t'<<split[3]<<'\t'<<std::endl;
			string_split(split[1], split_split, "/");
			split_split.push_back(split[1]);
			hlpr_ver.pos[0] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 0);
			hlpr_ver.pos[1] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 1);
			hlpr_ver.pos[2] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 2);
			
			hlpr_ver.u = texture_buffer.at(2 * (std::stoi(split_split[1]) - 1) + 0);
			hlpr_ver.v = texture_buffer.at(2 * (std::stoi(split_split[1]) - 1) + 1);
			
			hlpr_ver.nor[0] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 0);
			hlpr_ver.nor[1] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 1);
			hlpr_ver.nor[2] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 2);
			vertices.push_back(hlpr_ver);
			index_buffer.push_back(index_count);
			index_count++;
			split_split.clear();
			
			string_split(split[2], split_split, "/");
			split_split.push_back(split[2]);
			hlpr_ver.pos[0] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 0);
			hlpr_ver.pos[1] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 1);
			hlpr_ver.pos[2] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 2);
			
			hlpr_ver.u = texture_buffer.at(2 * (std::stoi(split_split[1])- 1) + 0);
			hlpr_ver.v = texture_buffer.at(2 * (std::stoi(split_split[1])- 1) + 1);
			
			hlpr_ver.nor[0] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 0);
			hlpr_ver.nor[1] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 1);
			hlpr_ver.nor[2] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 2);
			vertices.push_back(hlpr_ver);
			index_buffer.push_back(index_count);
			index_count++;
			split_split.clear();


			string_split(split[3], split_split, "/");
			split_split.push_back(split[3]);

			hlpr_ver.pos[0] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 0);
			hlpr_ver.pos[1] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 1);
			hlpr_ver.pos[2] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 2);
			
			hlpr_ver.u = texture_buffer.at(2 * (std::stoi(split_split[1])- 1) + 0);
			hlpr_ver.v = texture_buffer.at(2 * (std::stoi(split_split[1])- 1) + 1);
			
			hlpr_ver.nor[0] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 0);
			hlpr_ver.nor[1] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 1);
			hlpr_ver.nor[2] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 2);
			vertices.push_back(hlpr_ver);
			index_buffer.push_back(index_count);
			index_count++;
		}
	}
	file.close();

}

std::tuple<glm::mat4,glm::vec3> Plane::get_MVP_Matrix(float FOVdeg, float nearPlane, float farPlane, float aspect){
	glm::vec3 camera_dir_vec(0, -camera_up_distance, camera_behind_distant);
	glm::vec3 camera_pos = position - camera_dir_vec;
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
    glm::mat4 model = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(scaling_factor, scaling_factor, scaling_factor)), this->position);
	model = glm::rotate(model, glm::radians(yay_degree  ), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(pitch_degree), glm::vec3(-1.0f, 0.0f, 0.0f));
    shader.SetUniformMat4f(this->MVP_uniform_name, viewAndProjection * model);
    GLCall(glDrawElements(GL_TRIANGLES, ibo->GetCount() , GL_UNSIGNED_INT, nullptr));
}

void Plane::catchInputs(GLFWwindow* window){
    glm::vec3 orientation( cos(glm::radians(pitch_degree)) * sin(glm::radians(yay_degree)), 
	sin(glm::radians(pitch_degree)) ,
	 cos(glm::radians(pitch_degree)) * cos(glm::radians(yay_degree)));
	if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
		pitch_degree += rotation_angle_per_frame_deg;
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
		pitch_degree -= rotation_angle_per_frame_deg;
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		yay_degree += rotation_angle_per_frame_deg;
    }
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
		yay_degree -= rotation_angle_per_frame_deg;
    }
    position += speed * orientation;
	// printf("%s \n", glm::to_string(position).c_str());
}

Plane::Plane(const std::string& computeFile, const std::string& vertexFile, const std::string& fragFile, const std::string& modelObjFile,
	const std::string &texFile, int binding_Pnt,float camera_behind_distant,float camera_up_distance,float scaling_factor, const VertexBufferLayout &vertex_layout, const std::string MVP_uniform_name, const std::string &texture_UniformName):
vbo(nullptr), ibo(nullptr), collition_detection(computeFile), shader(vertexFile,
fragFile),camera_behind_distant(camera_behind_distant) ,camera_up_distance( camera_up_distance),  tex(texFile),MVP_uniform_name(MVP_uniform_name),  texture_UniformName(texture_UniformName),scaling_factor(scaling_factor) {
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