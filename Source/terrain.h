#pragma once

#include "../OpenGL/IndexBuffer.h"
#include "../OpenGL/Shader.h"
#include "../OpenGL/VertexArray.h"
#include "../OpenGL/VertexBuffer.h"
#include "../OpenGL/Texture.h"
#include "../OpenGL/VertexBufferLayout.h"
#include "vertex_index_layout.h"
#include "waveFrontFileApi.h"
#include "../IncGLM.hpp"
#include "../FOSS_Code/json.hpp"




class TerrainPatch{
    private:
        GLuint VBO;
        GLuint EBO;
        VertexArray vertex_array;
        VertexBuffer* vertex_buffer;
        IndexBuffer* index_buffer;
        ComputeShader terrain_generator;
        Shader shader;
        Texture tex;
        unsigned int texture_slot_m = 0;
        const int div_m; 
	    const float length_of_side_m;
        const float mountain_scale_factor_m;
        const glm::vec3 sun_dir_m;
        glm::vec2 terrain_index_m;
    public:
    TerrainPatch( 
        const std::string &terrainTextureFile,float fog_density , 
        const std::vector<int>  &ActiveWaveNumber, float rotation_angle_fractal_ground,
	    float output_increase_fctr_, float input_shrink_fctr_, 
        float lacunarity, float persistance,
	    bool writeToFile,int div, 
	    float length_of_side, 
	    float Mountain_Scale_Factor, 
        const glm::vec3 &sun_direction,
        glm::vec2 terrain_index,
        const bool async_generation,
        const std::string &terrainGeneratorShaderFile = "shaders/terrain/generate.glsl", 
	    const std::string &vertexShaderFile = "shaders/terrain/vertex.glsl",
	    const std::string &fragmentShaderFile = "shaders/terrain/fragment.glsl"
	);
    ~TerrainPatch();
    void render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos);

    int get_divisions() const;

    std::tuple<float, float, float,float> get_corners() const ;

    unsigned int get_terrain_ssbo_buffer_id() const;

    float get_Mountain_Scale() const;

    void sync();

    void set_index(
        const glm::vec2 &new_index,const std::vector<int>  &ActiveWaveNumber, float rotation_angle_fractal_ground,
	float output_increase_fctr_, float input_shrink_fctr_, 
    float lacunarity, float persistance, 
	float length_of_side, 
	float Mountain_Scale_Factor
    );
};



// constexpr int Terrain::NumberOfPatcherInGridPerAxis = 3;