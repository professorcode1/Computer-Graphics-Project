


#include "main.h"
#include "Cherno_OpenGL_Library/Camera.h"
#include "Cherno_OpenGL_Library/IndexBuffer.h"
#include "Cherno_OpenGL_Library/Shader.h"
#include "Cherno_OpenGL_Library/VertexArray.h"
#include "Cherno_OpenGL_Library/VertexBuffer.h"
#include "Cherno_OpenGL_Library/VertexBufferLayout.h"
#include <fstream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>



const unsigned int SCREEN_WIDTH = 1024;
const unsigned int SCREEN_HEIGHT = 1024;

const unsigned short OPENGL_MAJOR_VERSION = 4;
const unsigned short OPENGL_MINOR_VERSION = 6;

bool vSync = true;

struct vertex_t{
	float pos[4];
	float nor[4];
};
struct index_t{
	int indices[6];
};

void write_to_file(GLuint VBO,GLuint EBO,int div, bool write_normals, const char* file_name = "computeShaderResult.OBJ"){
	vertex_t* data_buffer = new vertex_t[( div + 1 ) * ( div + 1 )];
	index_t* indices_buffer = new index_t[ div * div ];
	glGetNamedBufferSubData(VBO, 0, ( div + 1 ) * ( div + 1 ) * sizeof(struct vertex_t), data_buffer);
	glGetNamedBufferSubData(EBO, 0, div * div * sizeof(struct index_t), indices_buffer);
	std::ofstream file(file_name);
	int progress = -1;
	for(int iter_i = 0 ; iter_i < ( div + 1 ) * ( div + 1 ) ; iter_i++){
		if(ceil((float) iter_i * 50 / (( div + 1 ) * ( div + 1 ))) > progress){
			progress++;
			printf("%d \n", progress);
		}
		file<<"v "<<data_buffer[iter_i].pos[0]<<" "<<data_buffer[iter_i].pos[1]<<" "<<data_buffer[iter_i].pos[2]<<'\n';
	}
	for(int iter_i = 0 ; iter_i < ( div + 1 ) * ( div + 1 ) ; iter_i++){
		if(ceil((float) 33.3 + iter_i * 50 / (( div + 1 ) * ( div + 1 ))) > progress){
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

	ComputeShader comp_shader("shader_compute.glsl");
	VertexArray vertex_array;
	VertexBuffer vertex_buffer(VBO);
	IndexBuffer index_buffer(EBO, div * div);
	VertexBufferLayout vertex_layout;
	Shader shader("shader_vertex.glsl", "shader_fragment.glsl");
	vertex_layout.Push<float>(4);
	vertex_layout.Push<float>(4);
	vertex_array.AddBuffer(vertex_buffer, vertex_layout);

	comp_shader.SetUniform1i("number_of_divs", div);
	comp_shader.SetUniform1f("min_x", min_x);
	comp_shader.SetUniform1f("max_x", max_x);
	comp_shader.SetUniform1f("min_z", min_z);
	comp_shader.SetUniform1f("max_z", max_z);
	int ActiveWaveFreqsGround = 0;
    for(int freq :parameter_json["wave numbers active"])
        ActiveWaveFreqsGround |= (1 << freq);
	std::cout<<"ActiveWaveFreqsGround\t"<<ActiveWaveFreqsGround<<std::endl;

	comp_shader.SetUniform1i("ActiveWaveFreqsGround", ActiveWaveFreqsGround);
	float rotation_angle_fractal_ground = parameter_json.at("rotation angle fractal ground");
	comp_shader.SetUniform1f("rotation_Angle", M_PI * rotation_angle_fractal_ground / 180.0f);
	comp_shader.SetUniform1f("output_increase_fctr", parameter_json.at("output_increase_fctr_"));
	comp_shader.SetUniform1f("input_shrink_fctr", parameter_json.at("input_shrink_fctr_"));
	comp_shader.SetUniform1f("lacunarity", parameter_json.at("lacunarity"));
	comp_shader.SetUniform1f("persistance", parameter_json.at("persistance"));
	comp_shader.bindSSOBuffer(0, vertex_buffer.GetRenderedID());
	comp_shader.bindSSOBuffer(1, index_buffer.GetRenderedID());
	// glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, VBO);
	// glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, EBO);
	comp_shader.launch_and_Sync(ceil((float)(div +1)/8), ceil((float)(div +1)/4), 1);
	shader.Bind();
	vertex_array.Bind();
	index_buffer.Bind();
	// GLCall(glDrawElements(GL_TRIANGLES, index_buffer.GetCount(), GL_UNSIGNED_INT, nullptr));

	write_to_file(VBO,EBO,div, true);
	std::cout<<min_x<<" "<< max_x<<" "<<  min_z<<" "<<  max_z<<std::endl;
	glfwSwapInterval(1);
	// glm::vec3 position(19.5574,13.7825,23.9988), focal_point(5,-0.03651,5),view_up(-0.316333,0.854152,-0.412745);
	glm::vec3 position (0,0.1716,0.5228);
	glm::vec3 focal_point (5,-0.02399,5);
	glm::vec3 view_up (-0.29207,0.897259,-0.331091);
	Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT, position);




	// const char* glsl_version = "#version 460";
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // // ImGuiIO& io = ImGui::GetIO(); (void)io;
    // //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // // Setup Dear ImGui style
    // // ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // // Setup Platform/Renderer backends
    // ImGui_ImplGlfw_InitForOpenGL(window, true);
    // ImGui_ImplOpenGL3_Init(glsl_version);

	while (!glfwWindowShouldClose(window))
	{	
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplGlfw_NewFrame();
        // ImGui::NewFrame();





		shader.Bind();

		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.Matrix(45.0f, 0.1f, 100.0f, shader, "camMatrix");

		vertex_array.Bind();
		index_buffer.Bind();
		GLCall(glDrawElements(GL_TRIANGLES, index_buffer.GetCount(), GL_UNSIGNED_INT, nullptr));
		glfwSwapBuffers(window);
		glfwPollEvents();
		
     	// {
        //     ImGui::Begin("Hello, world!");   
        //     // ImGui::SliderFloat3("Camera Position", &position.x, -100.0f, 100.0f);
        //     // ImGui::SliderFloat3("Camera Orientation", &focal_point.x, -100.0f, 100.0f);
        //     // ImGui::SliderFloat3("Camera Up", &view_up.x, -100.0f, 100.0f);
		// 	ImGui::InputFloat("Camera Position X",&position.x, -100.0f, 100.0f, "%.3f");
		// 	ImGui::InputFloat("Camera Position y",&position.y, -100.0f, 100.0f, "%.3f");
		// 	ImGui::InputFloat("Camera Position z",&position.z, -100.0f, 100.0f, "%.3f");

		// 	ImGui::InputFloat("Camera Orientation X",&focal_point.x, -100.0f, 100.0f, "%.3f");
		// 	ImGui::InputFloat("Camera Orientation y",&focal_point.y, -100.0f, 100.0f, "%.3f");
		// 	ImGui::InputFloat("Camera Orientation z",&focal_point.z, -100.0f, 100.0f, "%.3f");
            
		// 	ImGui::InputFloat("Camera Up X",&view_up.x, -100.0f, 100.0f, "%.3f");
		// 	ImGui::InputFloat("Camera Up y",&view_up.y, -100.0f, 100.0f, "%.3f");
		// 	ImGui::InputFloat("Camera Up z",&view_up.z, -100.0f, 100.0f, "%.3f");

		// 	ImGui::Text("Application average %.3f ms/frame(%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        //     ImGui::End();
        // }
		// ImGui::Render();
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		// glm::normalize(focal_point);
		// glm::normalize(view_up);
		// std::this_thread::sleep_for(std::chrono::milliseconds(100));

	}

	// ImGui_ImplOpenGL3_Shutdown();
	// ImGui_ImplGlfw_Shutdown();
	// ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
}