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
class Shader{
private:
    unsigned int m_RendererID;
    std::string m_filepathVertexShader, m_filepathFragmentShader;
    std::unordered_map<std::string, int> m_UniformLocationCache;
public:
    Shader(const std::string& filepathVertexShader, const std::string& filepathFragmentShader);
    Shader(const std::string& VertexShader, const std::string& FragmentShader, int some_number);
    ~Shader();
    
    void Bind() const ;
    void Unbind() const ;

    void SetUniform4f(const std::string &name,float v0, float v1, float v2, float v3);
    void SetUniform3f(const std::string &name,float v0, float v1, float v2);
    void SetUniform1f(const std::string &name,float v0);
    void SetUniform1i(const std::string &name,int v0);
    void SetUniformMat4f(const std::string &name, const glm::mat4& matrix);
private:
    void ParseShader(const std::string& vertexShader, const std::string& fragmentShader);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    int GetUniformLocation(const std::string &name);
};

class ComputeShader{
    private:
    unsigned int m_RendererID;
    std::string m_filepathComputeShader;
    std::unordered_map<std::string, int> m_UniformLocationCache;
    
    public:
    ComputeShader(const std::string& filepathComputeShader);
    ~ComputeShader();
    
    void Bind() const ;
    void Unbind() const ;

    void bindSSOBuffer(const int binding_point, const unsigned int buffer_ID);
    void launch_and_Sync(unsigned int x, unsigned int y, unsigned int z);

    void SetUniform4f(const std::string &name,float v0, float v1, float v2, float v3);
    void SetUniform1i(const std::string &name,int v0);
    void SetUniform2f(const std::string &name,float v0, float v1);
    void SetUniform1f(const std::string &name,float v0);
    void SetUniformMat4f(const std::string &name, const glm::mat4& matrix);
    
    inline unsigned int GetRenderedID() const {return m_RendererID;}
private:
    void ParseShader(const std::string& vertexShader, const std::string& fragmentShader);
    unsigned int CompileShader(const std::string& source);
    unsigned int CreateShader(const std::string& computeShader);
    int GetUniformLocation(const std::string &name);

};