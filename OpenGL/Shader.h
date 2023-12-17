#pragma once
#include <string>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
/*
Todo, make a shader base class with protected constructor to abstract out the 
uniforms and such
*/

class ShaderBase{
private:
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_UniformLocationCache;

    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    unsigned int CreateShader(const std::string& computeShader);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    int GetUniformLocation(const std::string &name);
protected:
    ShaderBase(const std::string& filepathVertexShader, const std::string& filepathFragmentShader);
    ShaderBase(const std::string& filepathComputeShader);
    ~ShaderBase();

public:
    void Bind() const ;
    void Unbind() const ;

    void SetUniform4f(const std::string &name,float v0, float v1, float v2, float v3);
    void SetUniform3f(const std::string &name,float v0, float v1, float v2);
    void SetUniform2f(const std::string &name,float v0, float v1);
    void SetUniform1f(const std::string &name,float v0);
    void SetUniform1i(const std::string &name,int v0);
    void SetUniform1ui(const std::string &name,uint32_t v0);
    void SetUniformMat4f(const std::string &name, const glm::mat4& matrix);
};


class Shader : public ShaderBase{
private:
    std::string m_filepathVertexShader, m_filepathFragmentShader;
public:
    Shader(const std::string& filepathVertexShader, const std::string& filepathFragmentShader);
    ~Shader();
};

class ComputeShader : public ShaderBase{
    private:
    std::string m_filepathComputeShader;
    
    public:
    ComputeShader(const std::string& filepathComputeShader);
    ~ComputeShader();
    
    void bindSSOBuffer(const int binding_point, const unsigned int buffer_ID);
    void launch_and_Sync(unsigned int x, unsigned int y, unsigned int z);
    void launch(unsigned int x, unsigned int y, unsigned int z);
    void sync();

};