#pragma once

#include "../OpenGL/IndexBuffer.h"
#include "../OpenGL/Shader.h"
#include "../OpenGL/VertexArray.h"
#include "../OpenGL/VertexBuffer.h"
#include "../OpenGL/Texture.h"
#include "../OpenGL/VertexBufferLayout.h"
#include "vertex_index_layout.h"
#include "waveFrontFileApi.h"

class Terrain{
    private:
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
        VertexArray vertex_array;
        VertexBuffer*  vertex_buffer;
        IndexBuffer* index_buffer;
        ComputeShader terrain_generator;
        Shader shader;
        Texture tex;
        glm::mat4 mountain_model;
    public:
    Terrain( const std::string &terrainGeneratorShaderFile, const std::string &vertexShaderFile, const std::string &fragmentShaderFile ,const VertexBufferLayout &vertex_layout,
    const std::string &terrainTextureFile,float fog_density , 
    const std::vector<int>  &ActiveWaveNumber, float rotation_angle_fractal_ground,float output_increase_fctr_, float input_shrink_fctr_, 
    float lacunarity, float persistance,bool writeToFile,int div, float min_x,float max_x,float min_z,float max_z, float Mountain_Scale_Factor);

    void render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos);
};