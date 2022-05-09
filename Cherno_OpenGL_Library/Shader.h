#pragma once
#include <string>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
    void SetUniform1i(const std::string &name,int v0);
    void SetUniformMat4f(const std::string &name, glm::mat4& matrix);
private:
    void ParseShader(const std::string& vertexShader, const std::string& fragmentShader);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    int GetUniformLocation(const std::string &name);
};