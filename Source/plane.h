#pragma once

#include "../OpenGL/IndexBuffer.h"
#include "../OpenGL/Shader.h"
#include "../OpenGL/VertexArray.h"
#include "../OpenGL/VertexBuffer.h"
#include "../OpenGL/Texture.h"
#include "../OpenGL/VertexBufferLayout.h"
#include "vertex_index_layout.h"
#include "waveFrontFileApi.h"
#include <cstddef>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include<GLFW/glfw3.h>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include <cstdio>
#include <glm/common.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/trigonometric.hpp>


class Plane{
    public:

        void set_position(glm::vec3 position){
            this->position = position;
        }
        void set_Speed(float speed){
            this->speed = speed;
        }

        void render(glm::mat4 viewAndProjection);

        void catchInputs(GLFWwindow* window);
        
        std::tuple<glm::mat4,glm::vec3> get_MVP_Matrix(float FOVdeg, float nearPlane, float farPlane ,float aspect);
        
        Plane(
        const std::string& modelObjFile, const std::string &texFile,
        const float rotation_x,const float rotation_y,const float rotation_z, 
        float camera_behind_distant,float camera_up_distance, 
        float camera_ViewPoint_distance,float scaling_factor,
        float speed, 
        const std::string MVP_uniform_name = "MVP_plane", const std::string &texture_UniformName = "plane_texture",
        const std::string& computeFile = "shaders/plane/collition.glsl", 
        const std::string& vertexFile = "shaders/plane/vertex.glsl", 
        const std::string& fragFile = "shaders/plane/fragment.glsl"
         );
    private:
        VertexArray vao;
        VertexBuffer* vbo;
        IndexBuffer* ibo;
        ComputeShader collition_detection;
        Shader shader;
        Texture tex;
        const int texture_BindSlot = 0;
        float scaling_factor;
        glm::vec3 position = glm::vec3(3,3,3);
        glm::vec3 Up = glm::vec3(0,1,0);
        float yay_degree = 0;
        float pitch_degree = 0;
        float roll_degree = 0;
        std::string MVP_uniform_name;
        std::string texture_UniformName;
        float speed = 0.10f;
        const float rotation_angle_per_frame_deg = 1.5f;
        float camera_behind_distant;
        float camera_up_distance;
        float camera_ViewPoint_distance;

};

