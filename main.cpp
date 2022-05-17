#include "main.h"
#include <iterator>
using json = nlohmann::json;
const unsigned short OPENGL_MAJOR_VERSION = 4;
const unsigned short OPENGL_MINOR_VERSION = 6;

const char* screenVertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec4 position;
uniform mat4 MVP;
void main(){
	gl_Position = MVP * position;
};)";

const char* screenFragmentShaderSource = R"(
#version 330 core

layout(location = 0) out vec4 color;

void main(){
	color = vec4(1.0, 0.0, 0.0, 0.0);
})";
struct params_for_gpu{
    int number_of_divs;
    float min_x;
    float max_x;
    float min_z;
    float max_z;
    int ActiveWaveFreqsGround;
    float rotation_Angle;
    int number_of_threads_engaged;
};
struct shader_data_t{
    float pos[4];
    float nor[4];
};
int main(){
    GLFWwindow* window;
    std::ifstream params("parameters.json");
    json parameters;
    params >> parameters;
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    
    const int width = parameters["screen width"], height = parameters["screen height"];
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        std::cout<<"Could not Create window"<<std::endl;
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if(glewInit() != GLEW_OK)
        std::cout<<"Error"<<std::endl;  

    std::cout<<glGetString(GL_VERSION) << std::endl;
    GLCall(glClearColor(0.5f, 0.6f, 0.7f, 0.0f));    
    Camera camera(width, height, 
        glm::vec3(parameters["camera position"][0], parameters["camera position"][1], parameters["camera position"][2]), 
        glm::vec3(parameters["camera orientation"][0], parameters["camera orientation"][1], parameters["camera orientation"][2]), 
        glm::vec3(parameters["camera up"][0], parameters["camera up"][1], parameters["camera up"][2]));
    glm::mat4 matrix_transform;
	float min_x, max_x, min_z, max_z;
    std::tie(matrix_transform, min_x, max_x, min_z, max_z) = camera.Matrix(parameters["field of view"], parameters["near plane"], parameters["far plane"], 
    parameters["write frustum to file"], parameters["padding"]);
    

    glViewport(0, 0, width, height);
    const int div = parameters["divisions"];
    const int size_of_each_vertex_bytes = sizeof(shader_data_t); //2 vec4s of 4 byte floats
	GLuint VAO, VBO, EBO, parametersIdGPU;
	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &EBO);
	glCreateBuffers(1, &parametersIdGPU);

	glNamedBufferData(VBO, ( div + 1 ) * ( div + 1 ) * size_of_each_vertex_bytes , NULL, GL_STATIC_DRAW);
	glNamedBufferData(EBO, div * div * 6 * 4 , NULL, GL_STATIC_DRAW);
    struct params_for_gpu params_for_gpu_;
    params_for_gpu_.number_of_divs = div;
    params_for_gpu_.ActiveWaveFreqsGround = 0;
    for(int freq :parameters["wave numbers active"])
        params_for_gpu_.ActiveWaveFreqsGround |= (1 << freq);
    params_for_gpu_.max_x = max_x;
    params_for_gpu_.min_x = min_x;
    params_for_gpu_.max_z = max_z;
    params_for_gpu_.min_z = min_z;
    params_for_gpu_.rotation_Angle = parameters["rotation angle fractal ground"];
    params_for_gpu_.number_of_threads_engaged = 0;
    glNamedBufferData(parametersIdGPU,sizeof(struct params_for_gpu), &params_for_gpu_, GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribBinding(VAO, 1, 0);
	glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));

	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 6 * sizeof(GLfloat));
	glVertexArrayElementBuffer(VAO, EBO);

	GLuint screenVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(screenVertexShader, 1, &screenVertexShaderSource, NULL);
	glCompileShader(screenVertexShader);
	GLuint screenFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(screenFragmentShader, 1, &screenFragmentShaderSource, NULL);
	glCompileShader(screenFragmentShader);

	GLuint screenShaderProgram = glCreateProgram();
	glAttachShader(screenShaderProgram, screenVertexShader);
	glAttachShader(screenShaderProgram, screenFragmentShader);
	glLinkProgram(screenShaderProgram);

	glDeleteShader(screenVertexShader);
	glDeleteShader(screenFragmentShader);



    std::ifstream computeShaderFile("shader_compute.glsl");

    std::string computeShaderStr((std::istreambuf_iterator<char>(computeShaderFile)), std::istreambuf_iterator<char>());
    const char* screenComputeShaderSource = computeShaderStr.c_str();
    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShader, 1, &screenComputeShaderSource, NULL);
	glCompileShader(computeShader);

    int result;
	glGetShaderiv(computeShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE){
			int length;
			glGetShaderiv(computeShader, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(computeShader, length, &length, message);
			std::cout<<"Failed to compile shader"<<std::endl;
			std::cout<<message<<std::endl;
			return 0;
	}

	GLuint computeProgram = glCreateProgram();
	glAttachShader(computeProgram, computeShader);
	glLinkProgram(computeProgram);

	int work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
	std::cout << "Max work groups per compute shader" << 
		" x:" << work_grp_cnt[0] <<
		" y:" << work_grp_cnt[1] <<
		" z:" << work_grp_cnt[2] << "\n";

	int work_grp_size[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
	std::cout << "Max work group sizes" <<
		" x:" << work_grp_size[0] <<
		" y:" << work_grp_size[1] <<
		" z:" << work_grp_size[2] << "\n";

	int work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	std::cout << "Max invocations count per work group: " << work_grp_inv << "\n";

    //attaching the PVM matrix
    GLCall(glUseProgram(screenShaderProgram));
    GLCall(int location = glGetUniformLocation(screenShaderProgram, "MVP"));
    glUniformMatrix4fv(location,1,GL_FALSE, &matrix_transform[0][0]);
    
    
    //reading result of compute shader
    glUseProgram(computeShader);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, VBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, EBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, parametersIdGPU);
    glDispatchCompute(ceil((float)(div+1) / 8), ceil((float)(div+1) / 4), 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    // glBindBuffer(GL_SHADER_STORAGE_BUFFER, VBO);
    // GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    // struct shader_data_t shader_data[( div + 1 ) * ( div + 1 )];
    // memcpy(p, &shader_data, ( div + 1 ) * ( div + 1 ) * size_of_each_vertex_bytes);
    // glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    // for(int iter_i = 0 ; iter_i < ( div + 1 ) * ( div + 1 ) ; iter_i++){
    //     std::cout<< iter_i<<std::endl;
    //     std::cout<<shader_data[iter_i].pos[0]<<" "<<shader_data[iter_i].pos[1]<<" "<<shader_data[iter_i].pos[2]<<std::endl;
    //     std::cout<<shader_data[iter_i].nor[0]<<" "<<shader_data[iter_i].nor[1]<<" "<<shader_data[iter_i].nor[2]<<std::endl;
    // }


    // glBindBuffer(GL_SHADER_STORAGE_BUFFER, EBO);
    // GLCall(GLvoid* p = glMapNamedBuffer(EBO, GL_READ_ONLY));
    // std::cout<<p<<std::endl;
    // int* shader_data = new int[div *div * 6];
    // for(int iter_i = 0 ; iter_i < div * div ; iter_i++){

    //     shader_data[6 * iter_i + 0 ] = 6 * iter_i + 0   ;
    //     shader_data[6 * iter_i + 1 ] = 6 * iter_i + 1   ;
    //     shader_data[6 * iter_i + 2 ] = 6 * iter_i + 2   ;
    //     shader_data[6 * iter_i + 3 ] = 6 * iter_i + 3   ;
    //     shader_data[6 * iter_i + 4 ] = 6 * iter_i + 4   ;
    //     shader_data[6 * iter_i + 5 ] = 6 * iter_i + 5   ;

    //     std::cout<<shader_data[6 * iter_i + 0 ]<<std::endl;
    //     std::cout<<shader_data[6 * iter_i + 1 ]<<std::endl;
    //     std::cout<<shader_data[6 * iter_i + 2 ]<<std::endl;
    //     std::cout<<shader_data[6 * iter_i + 3 ]<<std::endl;
    //     std::cout<<shader_data[6 * iter_i + 4 ]<<std::endl;
    //     std::cout<<shader_data[6 * iter_i + 5 ]<<std::endl;
    // }
    // memcpy(shader_data,p, div *div * 6 * 4);
    // glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    // std::cout<<"Post transfer"<<std::endl;
    // for(int iter_i = 0 ; iter_i < div * div ; iter_i++){
    //     std::cout<<shader_data[6 * iter_i + 0 ]<<std::endl;
    //     std::cout<<shader_data[6 * iter_i + 1 ]<<std::endl;
    //     std::cout<<shader_data[6 * iter_i + 2 ]<<std::endl;
    //     std::cout<<shader_data[6 * iter_i + 3 ]<<std::endl;
    //     std::cout<<shader_data[6 * iter_i + 4 ]<<std::endl;
    //     std::cout<<shader_data[6 * iter_i + 5 ]<<std::endl;
    // }


    glBindBuffer(GL_SHADER_STORAGE_BUFFER, parametersIdGPU);
    GLCall(GLvoid* p = glMapNamedBuffer(parametersIdGPU, GL_READ_ONLY));
    std::cout<<p<<std::endl;
    // struct params_for_gpu* params_for_gpu__ = (struct params_for_gpu*) p; 
    struct params_for_gpu params_for_gpu__;
    memcpy(&params_for_gpu__, p, sizeof(struct params_for_gpu));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    std::cout<<params_for_gpu__.ActiveWaveFreqsGround<<std::endl;
    std::cout<<params_for_gpu__.min_x<<std::endl;
    std::cout<<params_for_gpu__.max_x<<std::endl;
    std::cout<<params_for_gpu__.min_z<<std::endl;
    std::cout<<params_for_gpu__.max_z<<std::endl;
    std::cout<<params_for_gpu__.number_of_divs<<std::endl;
    std::cout<<params_for_gpu__.rotation_Angle<<std::endl;
    std::cout<<params_for_gpu__.number_of_threads_engaged<<std::endl;
    std::cout<<std::endl<<std::endl<<std::endl;
    std::cout<<params_for_gpu_.ActiveWaveFreqsGround<<std::endl;
    std::cout<<params_for_gpu_.min_x<<std::endl;
    std::cout<<params_for_gpu_.max_x<<std::endl;
    std::cout<<params_for_gpu_.min_z<<std::endl;
    std::cout<<params_for_gpu_.max_z<<std::endl;
    std::cout<<params_for_gpu_.number_of_divs<<std::endl;
    std::cout<<params_for_gpu_.rotation_Angle<<std::endl;
    std::cout<<params_for_gpu_.number_of_threads_engaged<<std::endl;



    while(!glfwWindowShouldClose(window)){

        GLCall(glClear(GL_COLOR_BUFFER_BIT));
        glUseProgram(computeShader);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, VBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, EBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, parametersIdGPU);
        glDispatchCompute(ceil((float)(div+1) / 8), ceil((float)(div+1) / 4), 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);


        GLCall(glUseProgram(screenShaderProgram));
        GLCall(glBindVertexArray(VAO));
        GLCall(glDrawElements(GL_TRIANGLES, div * div, GL_UNSIGNED_INT, nullptr));

        glfwSwapBuffers(window);

        glfwPollEvents();

    }

    glfwTerminate();
    return 0;
}