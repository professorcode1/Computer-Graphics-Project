#include "plane.h"
#include "waveFrontFileApi.h"

Plane::Plane(
	const std::string& modelObjFile,
	const std::string &texFile,
	float camera_behind_distant,float camera_up_distance, 
	float camera_ViewPoint_distance,float scaling_factor,
	float speed ,  float starting_height, 
	const std::vector<float> &rotation_angles,
    const std::vector<std::string> &rotation_axises,
	const std::string &texture_UniformName,
	const std::string& vertexFile, 
	const std::string& fragFile
	):
	vbo(nullptr), 
	ibo(nullptr), 
	shader(vertexFile,fragFile),
	camera_behind_distant(camera_behind_distant) ,
	camera_up_distance( camera_up_distance),
	camera_ViewPoint_distance(camera_ViewPoint_distance),  
	tex(texFile),
	texture_UniformName(texture_UniformName),
	scaling_factor(scaling_factor), 
	speed(speed),
	position(0,starting_height,0)
	{
	VertexBufferLayout vertex_layout_simple;
	CREATE_VERTEX_LAYOUT(vertex_layout_simple);

	std::vector<vertex_t> vertices_plane;
	std::vector<unsigned int> index_buffer_plane; 
	parse_complex_wavefront(modelObjFile, vertices_plane, index_buffer_plane);
	rotate_mesh_inplace(vertices_plane, rotation_angles, rotation_axises);
	vbo = new VertexBuffer(vertices_plane.data(), vertices_plane.size() * sizeof(vertex_t));
	ibo = new IndexBuffer(index_buffer_plane.data(), index_buffer_plane.size());
	vao.AddBuffer(*vbo, vertex_layout_simple);
	vao.AddElementBuffer(*ibo);
	shader.Bind();
	shader.SetUniform1i(texture_UniformName, texture_BindSlot);

	shader.Unbind();
	vao.Unbind();
	vbo->Unbind();
	ibo->Unbind();
}

std::tuple<glm::mat4,glm::vec3> Plane::get_MVP_Matrix(){
	glm::vec3 orientation( cos(glm::radians(pitch_degree)) * sin(glm::radians(yay_degree)), 
	sin(glm::radians(pitch_degree)) ,
	 cos(glm::radians(pitch_degree)) * cos(glm::radians(yay_degree)));
	// std::cout<<"Orientation ::\t"<< glm::to_string(orientation)<<std::endl ;
	glm::vec3 view_position = camera_ViewPoint_distance * orientation + position;
	const glm::vec3 camera_pos = position +  camera_behind_distant * orientation * -1.0f + 
		glm::normalize(glm::cross(glm::cross(orientation, Up), orientation)) * camera_up_distance;
	glm::vec3 camera_dir_vec = glm::normalize(view_position - camera_pos);
	glm::mat4 view = glm::lookAt(camera_pos, camera_pos + glm::normalize(camera_dir_vec), Up);

	return {view, camera_pos};	
}

void Plane::render(glm::mat4 viewAndProjection){
    vao.Bind();
    shader.Bind();
    shader.SetUniform1i(this->texture_UniformName, texture_BindSlot);
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), this->position), glm::vec3(scaling_factor, scaling_factor, scaling_factor));
	model = glm::rotate(model, glm::radians(yay_degree  ), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(pitch_degree), glm::vec3(-1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(roll_degree  ), glm::vec3(0.0f, 0.0f, 1.0f));
    shader.SetUniformMat4f("MVP_plane", viewAndProjection * model);
	tex.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ibo->GetCount() , GL_UNSIGNED_INT, nullptr));
	tex.Unbind();
	shader.Unbind();
	vao.Unbind();
	vbo->Unbind();
	ibo->Unbind();
}

void Plane::catchInputs(int key){
    glm::vec3 orientation( cos(glm::radians(pitch_degree)) * sin(glm::radians(yay_degree)), 
	sin(glm::radians(pitch_degree)) ,
	 cos(glm::radians(pitch_degree)) * cos(glm::radians(yay_degree)));
	static bool roll_was_changed = false;
	if(key == GLUT_KEY_UP){
		pitch_degree = std::min(pitch_degree+rotation_angle_per_frame_deg, 60.0f);

    }
    if(key == GLUT_KEY_DOWN){
		pitch_degree = std::max(pitch_degree - rotation_angle_per_frame_deg, -60.0f);
    }
    if(key == GLUT_KEY_LEFT){
		roll_was_changed = true;
		roll_degree -= rotation_angle_per_frame_deg; 
		yay_degree += rotation_angle_per_frame_deg;
    }
    else if(key == GLUT_KEY_RIGHT){
		roll_was_changed = true;
		roll_degree += rotation_angle_per_frame_deg; 
		yay_degree -= rotation_angle_per_frame_deg;
    }else{
		roll_was_changed = false;
	}
    position += speed * orientation;
	if(! roll_was_changed && roll_was_changed > 360.0f){
		roll_degree = glm::mod(roll_degree, 360.0f);
	}
	if(! roll_was_changed  && roll_degree >= 1.0f){
			roll_degree -= rotation_angle_per_frame_deg/2.0;
	}

	if(! roll_was_changed  && roll_degree <= -1.0f){
			roll_degree += rotation_angle_per_frame_deg/2.0;
	}
	// printf("%s \n", glm::to_string(position).c_str());
}

glm::vec3 Plane::get_position() const {
	return this->position;
}