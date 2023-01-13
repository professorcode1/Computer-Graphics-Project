#include "Shader.h"
#include "Renderer.h"


unsigned int Shader::CompileShader(unsigned int type, const std::string& source){
    GLCall(unsigned int id = glCreateShader(type));
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));
	
	//error handeling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE){
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(id, length, &length, message);
			std::cout<<"Failed to compile shader"<<std::endl;
            std::cout<<((type == GL_VERTEX_SHADER) ? "Vertex Shader" : "Fragment Shader")<<std::endl;
			std::cout<<message<<std::endl;
			return 0;
	}
	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
	GLCall(unsigned int program = glCreateProgram());
	GLCall(unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader));
	GLCall(unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader));
	
	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));
	
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));
	return program;
}

Shader::Shader(const std::string& filepathVertexShader, const std::string& filepathFragmentShader):
m_RendererID(0) , m_filepathFragmentShader(filepathFragmentShader), m_filepathVertexShader(filepathVertexShader) {
    // std::cout<<filepathVertexShader<<std::endl;
    // std::cout<<filepathFragmentShader<<std::endl;
    std::ifstream vertexShaderFile(filepathVertexShader);
    std::ifstream fragmentShaderFile(filepathFragmentShader);
    std::string vertexShader((std::istreambuf_iterator<char>(vertexShaderFile)), std::istreambuf_iterator<char>());
    std::string fragmentShader((std::istreambuf_iterator<char>(fragmentShaderFile)), std::istreambuf_iterator<char>());
    // std::cout<<vertexShader<<std::endl;
    // std::cout<<fragmentShader<<std::endl;
    m_RendererID = CreateShader(vertexShader, fragmentShader);
}
Shader::Shader(const std::string& VertexShader, const std::string& FragmentShader, int some_number){
	//the number is just an excuse to overload the constructor to allow shaders from raw c++ strings
	m_RendererID = CreateShader(VertexShader, FragmentShader);
}

Shader::~Shader(){
    GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const{
    GLCall(glUseProgram(m_RendererID));
}
void Shader::Unbind() const {
    GLCall(glUseProgram(0));
}
void Shader::SetUniform4f(const std::string &name,float v0, float v1, float v2, float v3){
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniform3f(const std::string &name,float v0, float v1, float v2){
	GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}

void Shader::SetUniform1f(const std::string &name,float v0){
	GLCall(glUniform1f(GetUniformLocation(name), v0));
}

int Shader::GetUniformLocation(const std::string &name){
    // std::cout<<m_RendererID<<std::endl;
	if(m_UniformLocationCache.find(name) != m_UniformLocationCache.end()){
		return m_UniformLocationCache[name];
	}
    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if(location == -1)
        std::cout<<"Warning : uniform '" << name<< "' doesn't exist"<<std::endl;
    m_UniformLocationCache[name] = location;
	return location;
}
void Shader::SetUniform1i(const std::string &name,int v0){
    GLCall(glUniform1i(GetUniformLocation(name), v0));
}
void Shader::SetUniformMat4f(const std::string &name, const glm::mat4& matrix){
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}








unsigned int ComputeShader::CompileShader(const std::string& source){
    GLCall(unsigned int id = glCreateShader(GL_COMPUTE_SHADER));
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));
	
	//error handeling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE){
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(id, length, &length, message);
			std::cout<<"Failed to compile Compute shader"<<std::endl;
			std::cout<<message<<std::endl;
			return 0;
	}
	return id;
}

unsigned int ComputeShader::CreateShader(const std::string& computeShader){
	GLCall(unsigned int program = glCreateProgram());
	GLCall(unsigned int cs = CompileShader(computeShader));
	
	GLCall(glAttachShader(program, cs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));
	
	GLCall(glDeleteShader(cs));
	return program;
}

ComputeShader::ComputeShader(const std::string& filepathComputeShader):
m_RendererID(0) , m_filepathComputeShader(filepathComputeShader) {
    std::ifstream computeShaderFile(filepathComputeShader);
    std::string computeShaderStr((std::istreambuf_iterator<char>(computeShaderFile)), std::istreambuf_iterator<char>());
    // std::cout<<computeShaderStr<<std::endl;
    // std::cout<<fragmentShader<<std::endl;
	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
	
	const char * screenComputeShaderSource = computeShaderStr.c_str();
	// std::cout<<screenComputeShaderSource<<std::endl;
	GLCall(glShaderSource(computeShader, 1, &screenComputeShaderSource, NULL));
	GLCall(glCompileShader(computeShader));
  	int result;
	glGetShaderiv(computeShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE){
			int length;
			glGetShaderiv(computeShader, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(computeShader, length, &length, message);
			std::cout<<"Failed to compile shader"<<std::endl;
			std::cout<<message<<std::endl;
			assert(false);
	}

	GLuint computeProgram = glCreateProgram();
	GLCall(glAttachShader(computeProgram, computeShader));
	GLCall(glLinkProgram(computeProgram));
	GLCall(glUseProgram(computeProgram));
	m_RendererID = computeProgram;
}
ComputeShader::~ComputeShader(){
    GLCall(glDeleteProgram(m_RendererID));
}

void ComputeShader::Bind() const{
    GLCall(glUseProgram(m_RendererID));
}
void ComputeShader::Unbind() const {
    GLCall(glUseProgram(0));
}
void ComputeShader::SetUniform4f(const std::string &name,float v0, float v1, float v2, float v3){
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}
int ComputeShader::GetUniformLocation(const std::string &name){
    // std::cout<<m_RendererID<<std::endl;
	if(m_UniformLocationCache.find(name) != m_UniformLocationCache.end()){
		return m_UniformLocationCache[name];
	}
    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if(location == -1)
        std::cout<<"Warning : uniform '" << name<< "' doesn't exist"<<std::endl;
    m_UniformLocationCache[name] = location;
	return location;
}
void ComputeShader::SetUniform1i(const std::string &name,int v0){
    GLCall(glUniform1i(GetUniformLocation(name), v0));
}
void ComputeShader::SetUniform1f(const std::string &name,float v0){
    GLCall(glUniform1f(GetUniformLocation(name), v0));
}
void ComputeShader::SetUniformMat4f(const std::string &name, const glm::mat4& matrix){
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}


void ComputeShader::bindSSOBuffer(const int binding_point, const unsigned int buffer_ID){
	GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, buffer_ID));
}

void ComputeShader::launch_and_Sync(unsigned int x, unsigned int y, unsigned int z){
	using namespace std::chrono;
	this->Bind();
	auto start = high_resolution_clock::now();
	GLCall(glDispatchCompute(x,y,z));
	GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	std::cout<< "Compute Shader time taken in microseconds :: \t" << duration.count() << std::endl;

}
