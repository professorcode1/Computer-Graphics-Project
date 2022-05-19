


#include "main.h"
#include "Cherno_OpenGL_Library/IndexBuffer.h"
#include "Cherno_OpenGL_Library/Shader.h"
#include "Cherno_OpenGL_Library/VertexBuffer.h"
#include <fstream>



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

void write_to_file(GLuint VBO,GLuint EBO,int div, const char* file_name = "computeShaderResult.OBJ"){
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
	for(int iter_i = 0 ; iter_i < div * div ; iter_i++){
		if(ceil(50 + 50 * (float) iter_i  / (div*div)) > progress){
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

	Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT, 
        glm::vec3(parameter_json.at("camera position").at(0), parameter_json.at("camera position").at(1), 
		parameter_json.at("camera position").at(2)), glm::vec3(parameter_json.at("camera orientation").at(0), 
		parameter_json.at("camera orientation").at(1), parameter_json.at("camera orientation").at(2)), 
        glm::vec3(parameter_json.at("camera up").at(0), parameter_json.at("camera up").at(1), parameter_json.at("camera up").at(2)));
 	glm::mat4 matrix_transform;
	float min_x, max_x, min_z, max_z;
    std::tie(matrix_transform, min_x, max_x, min_z, max_z) = camera.Matrix(parameter_json.at("field of view"), 
	parameter_json.at("near plane"), parameter_json.at("far plane"), parameter_json.at("write frustum to file"), 
	parameter_json.at("padding"));
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
	VertexBuffer vertex_buffer(VBO);
	IndexBuffer index_buffer(EBO, div * div);
	GLuint computeProgram = comp_shader.GetRenderedID();

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
	// glDispatchCompute(ceil((float)(div +1)/8), ceil((float)(div +1)/4), 1);
	// glMemoryBarrier(GL_ALL_BARRIER_BITS);
	
	write_to_file(VBO,EBO,div);
	std::cout<<min_x<<" "<< max_x<<" "<<  min_z<<" "<<  max_z<<std::endl;

	// for(int iter_i = 0 ; iter_i < parameter_json.at("iter_i_lim") ; iter_i ++){
	// 	std::cout<<data_buffer[iter_i].pos[0]<<" "<<data_buffer[iter_i].pos[1]<<" "<<data_buffer[iter_i].pos[2]<<" "<<data_buffer[iter_i].pos[3]<<std::endl;
	// 	std::cout<<data_buffer[iter_i].nor[0]<<" "<<data_buffer[iter_i].nor[1]<<" "<<data_buffer[iter_i].nor[2]<<" "<<data_buffer[iter_i].nor[3]<<std::endl;
	// 	std::cout<<indices_buffer[iter_i].indices[0]<<" "<<indices_buffer[iter_i].indices[1]<<" "<<indices_buffer[iter_i].indices[2]<<" "<<indices_buffer[iter_i].indices[3]<<" "<<indices_buffer[iter_i].indices[4]<<" "<<indices_buffer[iter_i].indices[5]<<std::endl<<std::endl;
	// }

	// while (!glfwWindowShouldClose(window))
	// {
	// 	glUseProgram(computeProgram);
    //     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, params_ID);
    //     int ans[2];
    //     glGetNamedBufferSubData(params_ID,0,8,ans);
    //     std::cout<<ans[0]<<" "<<ans[1]<<std::endl;
	// 	glDispatchCompute(ceil(SCREEN_WIDTH / 8), ceil(SCREEN_HEIGHT / 4), 1);
	// 	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	// 	glUseProgram(screenShaderProgram);
	// 	glBindTextureUnit(0, screenTex);
	// 	glUniform1i(glGetUniformLocation(screenShaderProgram, "screen"), 0);
	// 	glBindVertexArray(VAO);
	// 	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

	// 	glfwSwapBuffers(window);
	// 	glfwPollEvents();
	// }


	glfwDestroyWindow(window);
	glfwTerminate();
}