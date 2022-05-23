


#include "main.h"
#include "Cherno_OpenGL_Library/IndexBuffer.h"
#include "Cherno_OpenGL_Library/VertexArray.h"
#include "Cherno_OpenGL_Library/VertexBuffer.h"
#include "Cherno_OpenGL_Library/VertexBufferLayout.h"
#include <fstream>
#include <glm/ext/matrix_transform.hpp>
#include <ostream>
#include <string>


const unsigned int SCREEN_WIDTH = 1024;
const unsigned int SCREEN_HEIGHT = 1024;

const unsigned short OPENGL_MAJOR_VERSION = 4;
const unsigned short OPENGL_MINOR_VERSION = 6;

bool vSync = true;

struct vertex_t{
	float pos[3];
	float u;
	float nor[3];
	float v;
};
struct index_t{
	int indices[6];
};

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

void write_to_file(GLuint VBO,GLuint EBO,int div, bool write_normals, const char* file_name = "computeShaderResult.OBJ"){
	vertex_t* data_buffer = new vertex_t[( div + 1 ) * ( div + 1 )];
	index_t* indices_buffer = new index_t[ div * div ];
	glGetNamedBufferSubData(VBO, 0, ( div + 1 ) * ( div + 1 ) * sizeof(struct vertex_t), data_buffer);
	glGetNamedBufferSubData(EBO, 0, div * div * sizeof(struct index_t), indices_buffer);
	std::ofstream file(file_name);
	int progress = -1;
	for(int iter_i = 0 ; iter_i < ( div + 1 ) * ( div + 1 ) ; iter_i++){
		if(ceil((float) iter_i * 33.3f / (( div + 1 ) * ( div + 1 ))) > progress){
			progress++;
			printf("%d \n", progress);
		}
		file<<"v "<<data_buffer[iter_i].pos[0]<<" "<<data_buffer[iter_i].pos[1]<<" "<<data_buffer[iter_i].pos[2]<<'\n';
	}
	for(int iter_i = 0 ; iter_i < ( div + 1 ) * ( div + 1 ) * write_normals ; iter_i++){
		if(ceil(33.3f + (float)iter_i * 33.3f / (( div + 1 ) * ( div + 1 ))) > progress){
			progress++;
			printf("%d \n", progress);
		}
		file<<"vn "<<data_buffer[iter_i].nor[0]<<" "<<data_buffer[iter_i].nor[1]<<" "<<data_buffer[iter_i].nor[2]<<'\n';
	}

	for(int iter_i = 0 ; iter_i < div * div ; iter_i++){
		if(ceil(66.6 + 33 * (float) iter_i  / (div*div)) > progress){
			progress++;
			printf("%d \n", progress);
		}

		file<<"f "<<indices_buffer[iter_i].indices[0] + 1 <<" "<<indices_buffer[iter_i].indices[1] + 1 <<" "<<indices_buffer[iter_i].indices[2] + 1 <<'\n';
		file<<"f "<<indices_buffer[iter_i].indices[3] + 1 <<" "<<indices_buffer[iter_i].indices[4] + 1 <<" "<<indices_buffer[iter_i].indices[5] + 1 <<'\n';
	}
	file.close();
}


int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Compute Shaders", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create the GLFW window\n";
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(vSync);
	// GLCall(glEnable(GL_BLEND));
	// GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    if(glewInit() != GLEW_OK){
        std::cout<<"Error"<<std::endl;  
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	std::ifstream parameter_file("parameters.json");
	json parameter_json;
	parameter_file >> parameter_json;
	float min_x, max_x, min_z, max_z;
	min_x = parameter_json.at("min_x");
	max_x = parameter_json.at("max_x");
	min_z = parameter_json.at("min_z");
	max_z = parameter_json.at("max_z");
	GLuint VAO, VBO, EBO;
	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &EBO);
	
	int div = parameter_json["divisions"];
	glNamedBufferData(VBO, ( div + 1 ) * ( div + 1 ) * sizeof(struct vertex_t) , NULL, GL_STATIC_DRAW);
	glNamedBufferData(EBO, div * div * 6 * 4, NULL, GL_STATIC_DRAW);

	ComputeShader basic_scene_generator("shader_compute.glsl");
	VertexArray vertex_array;
	VertexBuffer vertex_buffer(VBO);
	IndexBuffer index_buffer(EBO, div * div * 6);
	VertexBufferLayout vertex_layout;
	Shader shader("shader_vertex.glsl", "shader_fragment.glsl");
	shader.Bind();
	Texture tex(parameter_json.at("noise texture file"));
	tex.Bind();
	shader.SetUniform1i("u_Texture", 0);
	shader.SetUniform1f("atmosphere_light_damping_constant", parameter_json.at("atmosphere light damping constant")); 
	shader.SetUniform1f("atmosphere_light_damping_constant", parameter_json.at("atmosphere light damping constant")); 
	shader.SetUniform4f("atmosphere_light_damping_RGB_Weight", parameter_json.at("atmosphere red"), parameter_json.at("atmosphere green"), parameter_json.at("atmosphere blue"), 1.0); 
	vertex_layout.Push<float>(3);
	vertex_layout.Push<float>(1);
	vertex_layout.Push<float>(3);
	vertex_layout.Push<float>(1);
	vertex_array.AddBuffer(vertex_buffer, vertex_layout);

	basic_scene_generator.Bind();
	basic_scene_generator.SetUniform1i("number_of_divs", div);
	basic_scene_generator.SetUniform1f("min_x", min_x);
	basic_scene_generator.SetUniform1f("max_x", max_x);
	basic_scene_generator.SetUniform1f("min_z", min_z);
	basic_scene_generator.SetUniform1f("max_z", max_z);
	int ActiveWaveFreqsGround = 0;
    for(int freq :parameter_json["wave numbers active"]){
        ActiveWaveFreqsGround |= (1 << freq);
	}
	std::cout<<"ActiveWaveFreqsGround\t"<<ActiveWaveFreqsGround<<std::endl;

	basic_scene_generator.SetUniform1i("ActiveWaveFreqsGround", ActiveWaveFreqsGround);
	float rotation_angle_fractal_ground = parameter_json.at("rotation angle fractal ground");
	basic_scene_generator.SetUniform1f("rotation_Angle", M_PI * rotation_angle_fractal_ground / 180.0f);
	basic_scene_generator.SetUniform1f("output_increase_fctr", parameter_json.at("output_increase_fctr_"));
	basic_scene_generator.SetUniform1f("input_shrink_fctr", parameter_json.at("input_shrink_fctr_"));
	basic_scene_generator.SetUniform1f("lacunarity", parameter_json.at("lacunarity"));
	basic_scene_generator.SetUniform1f("persistance", parameter_json.at("persistance"));
	basic_scene_generator.bindSSOBuffer(0, vertex_buffer.GetRenderedID());
	basic_scene_generator.bindSSOBuffer(1, index_buffer.GetRenderedID());
	// glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, VBO);
	// glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, EBO);
	basic_scene_generator.launch_and_Sync(ceil((float)(div +1)/8), ceil((float)(div +1)/4), 1);
	vertex_array.Bind();
	index_buffer.Bind();
	// GLCall(glDrawElements(GL_TRIANGLES, index_buffer.GetCount(), GL_UNSIGNED_INT, nullptr));
	if(parameter_json.at("write to file"))
		write_to_file(VBO,EBO,div, parameter_json.at("write normals"));
	std::cout<<min_x<<" "<< max_x<<" "<<  min_z<<" "<<  max_z<<std::endl;
	glfwSwapInterval(1);
	// glm::vec3 position(19.5574,13.7825,23.9988), focal_point(5,-0.03651,5),view_up(-0.316333,0.854152,-0.412745);
	glm::vec3 position (0,0.1716,0.5228);
	glm::vec3 focal_point (5,-0.02399,5);
	glm::vec3 view_up (-0.29207,0.897259,-0.331091);
	glm::vec3 plane_Position(3,3,3);
	Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT, position, plane_Position);




	//glClearColor(135.0f / 225.0f, 206.0f / 225.0f, 235.0f / 225.0f, 1.0f);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	basic_scene_generator.launch_and_Sync(ceil((float)(div +1)/8), ceil((float)(div +1)/4), 1);
	std::vector<vertex_t> vertices_plane;
	std::vector<unsigned int> index_buffer_plane; 
	parse_simple_wavefront("assets/LowPolyPlane01/LowPolyPlane01.obj", vertices_plane, index_buffer_plane);
	VertexArray plane_vao;
	VertexBuffer plane_vbo(vertices_plane.data(), vertices_plane.size() * sizeof(vertex_t));
	IndexBuffer plane_EBO(index_buffer_plane.data(), index_buffer_plane.size());
	plane_vao.AddBuffer(plane_vbo, vertex_layout);
	Shader plane_shader("shader_vertex_plane.glsl", "shader_fragment_plane.glsl");
	plane_shader.Bind();
	Texture plane_tex("assets/low-poly-plane/textures/Plane_diffuse.png");
	plane_tex.Bind(1);
	plane_shader.SetUniform1i("u_Texture", 1);
	
	while (!glfwWindowShouldClose(window))
	{	

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Inputs(window);
		glm::mat4 MVP, MVP_plane;
		glm::vec3 camera_pos;
		std::tie(MVP, camera_pos, MVP_plane) = camera.Matrix(45.0f, 0.1f, 100.0f);
		
		plane_shader.Bind();
		plane_shader.SetUniformMat4f("MVP", MVP_plane);
		plane_vbo.Bind(); 
		plane_EBO.Bind();
		GLCall(glDrawElements(GL_TRIANGLES, plane_EBO.GetCount() , GL_UNSIGNED_INT, nullptr));

		// shader.Bind();
		// shader.SetUniformMat4f("MVP",MVP);
		// shader.SetUniform3f("camera_loc", camera_pos.x, camera_pos.y, camera_pos.z);
		// vertex_array.Bind();
		// index_buffer.Bind();

		GLCall(glDrawElements(GL_TRIANGLES, index_buffer.GetCount() , GL_UNSIGNED_INT, nullptr));


		glfwSwapBuffers(window);
		glfwPollEvents();
		

	}

	glfwDestroyWindow(window);
	glfwTerminate();
}