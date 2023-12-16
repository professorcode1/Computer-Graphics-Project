#include "Shader.h"
#include "Renderer.h"


unsigned int ShaderBase::CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
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

unsigned int ShaderBase::CreateShader(const std::string& computeShader){
	GLCall(unsigned int program = glCreateProgram());
	GLCall(unsigned int cs = CompileShader( GL_COMPUTE_SHADER, computeShader));
	
	GLCall(glAttachShader(program, cs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));
	
	GLCall(glDeleteShader(cs));
	return program;
}

unsigned int ShaderBase::CompileShader(unsigned int type, const std::string& source){
    GLCall(unsigned int id = glCreateShader(type));
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	char* shader_name;
	switch (type)
	{
		case GL_VERTEX_SHADER:
			shader_name = "Vertex Shader";
			break;
		case GL_FRAGMENT_SHADER:
			shader_name = "Fragment Shader";
			break;
		case GL_COMPUTE_SHADER:
			shader_name = "Compute Shader";
			break;
		default:
			std::cout<<"Unknown Shader type encountered in compile"<<std::endl;
			assert(false);
			break;
	}
	
	//error handeling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE){
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(id, length, &length, message);
			std::cout<<"Failed to compile "<<std::endl;
            std::cout<<shader_name<<std::endl;
			std::cout<<message<<std::endl;
			return 0;
	}
	return id;
}

int ShaderBase::GetUniformLocation(const std::string &name){
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

ShaderBase::~ShaderBase(){
    GLCall(glDeleteProgram(m_RendererID));
}

void ShaderBase::Bind() const{
    GLCall(glUseProgram(m_RendererID));
}
void ShaderBase::Unbind() const {
    GLCall(glUseProgram(0));
}
void ShaderBase::SetUniform4f(const std::string &name,float v0, float v1, float v2, float v3){
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void ShaderBase::SetUniform3f(const std::string &name,float v0, float v1, float v2){
	GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}
void ShaderBase::SetUniform2f(const std::string &name,float v0, float v1){
	GLCall(glUniform2f(GetUniformLocation(name), v0, v1));
}
void ShaderBase::SetUniform1f(const std::string &name,float v0){
	GLCall(glUniform1f(GetUniformLocation(name), v0));
}

void ShaderBase::SetUniform1i(const std::string &name,int v0){
    GLCall(glUniform1i(GetUniformLocation(name), v0));
}
void ShaderBase::SetUniformMat4f(const std::string &name, const glm::mat4& matrix){
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}


ShaderBase::ShaderBase(const std::string& filepathVertexShader, const std::string& filepathFragmentShader){
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

ShaderBase::ShaderBase(const std::string& filepathComputeShader){
	std::ifstream computeShaderFile(filepathComputeShader);
    std::string computeShaderStr((std::istreambuf_iterator<char>(computeShaderFile)), std::istreambuf_iterator<char>());
    m_RendererID = CreateShader(computeShaderStr);

}


Shader::Shader(const std::string& filepathVertexShader, const std::string& filepathFragmentShader):
	ShaderBase(filepathVertexShader, filepathFragmentShader), 
	m_filepathFragmentShader(filepathFragmentShader), 
	m_filepathVertexShader(filepathVertexShader) {}


Shader::~Shader(){}



ComputeShader::ComputeShader(const std::string& filepathComputeShader):
	ShaderBase(filepathComputeShader),
	m_filepathComputeShader(filepathComputeShader)
	{}

ComputeShader::~ComputeShader(){}

void ComputeShader::bindSSOBuffer(const int binding_point, const unsigned int buffer_ID){
	GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, buffer_ID));
}

void ComputeShader::launch_and_Sync(unsigned int x, unsigned int y, unsigned int z){
	using namespace std::chrono;
	auto start = high_resolution_clock::now();
	this->launch(x,y,z);
	this->sync();
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	std::cout<< "Compute Shader time taken in microseconds :: \t" << duration.count() << std::endl;

}

void ComputeShader::launch(unsigned int x, unsigned int y, unsigned int z){
	this->Bind();
	GLCall(glDispatchCompute(x,y,z));
}

void ComputeShader::sync(){
	GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));
}
