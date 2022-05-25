


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
	shader.SetUniform1i("mountain_tex", 0);
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
	basic_scene_generator.launch_and_Sync(ceil((float)(div +1)/8), ceil((float)(div +1)/4), 1);
	
	if(parameter_json.at("write to file"))
		write_to_file(VBO,EBO,div, parameter_json.at("write normals"));
	std::cout<<min_x<<" "<< max_x<<" "<<  min_z<<" "<<  max_z<<std::endl;
	
	glfwSwapInterval(1);
	
	glm::vec3 position (0,0.1716,0.5228);
	glm::vec3 focal_point (5,-0.02399,5);
	glm::vec3 view_up (-0.29207,0.897259,-0.331091);
	glm::vec3 plane_Position(3,3,3);




	//glClearColor(135.0f / 225.0f, 206.0f / 225.0f, 235.0f / 225.0f, 1.0f);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	Plane plane("shader_collition.glsl", "shader_vertex_plane.glsl", "shader_fragment_plane.glsl", parameter_json.at("Plane OBJ file"),
	parameter_json.at("Plane Texuture file"), 1, parameter_json.at("Camera Beind Distance"),
	 parameter_json.at("Camera Up Distance"), parameter_json.at("Plane Scale"), vertex_layout);
	
	while (!glfwWindowShouldClose(window))
	{	

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 VP, MVP_plane;
		glm::vec3 camera_pos;
		std::tie(VP, camera_pos) = plane.get_MVP_Matrix(parameter_json.at("FOV"), parameter_json.at("Near Plane"), 
		parameter_json.at("Far Plane"), (float)SCREEN_WIDTH / SCREEN_HEIGHT);
		
		shader.Bind();
		glm::mat4  mountain_model(glm::scale(glm::mat4(1.0f), glm::vec3(10,10,10)));
		shader.SetUniformMat4f("MVP_mountain",VP * mountain_model);
		tex.Bind();
		shader.SetUniform1i("mountain_tex", 0);
	
		shader.SetUniform3f("camera_loc", camera_pos.x, camera_pos.y, camera_pos.z);
		vertex_array.Bind();
		index_buffer.Bind();

		GLCall(glDrawElements(GL_TRIANGLES, index_buffer.GetCount() , GL_UNSIGNED_INT, nullptr));

		plane.catchInputs(window);
		plane.render(VP);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwDestroyWindow(window);
	glfwTerminate();
}