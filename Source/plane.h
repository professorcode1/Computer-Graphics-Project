#pragma once

#include "../OpenGL/IndexBuffer.h"
#include "../OpenGL/Shader.h"
#include "../OpenGL/VertexArray.h"
#include "../OpenGL/VertexBuffer.h"
#include "../OpenGL/Texture.h"
#include "../OpenGL/VertexBufferLayout.h"
#include "vertex_index_layout.h"
#include "waveFrontFileApi.h"
#include "meshTransform.h"
#include <cstddef>
#include<GLFW/glfw3.h>
#include <cstdio>
#include "../IncGLM.hpp"


class Plane{
    public:

        void set_position(glm::vec3 position){
            this->position = position;
        }
        void set_Speed(float speed){
            this->speed = speed;
        }
        glm::vec3 get_position() const;

        void render(glm::mat4 viewAndProjection);

        void catchInputs(GLFWwindow* window);
        
        std::tuple<glm::mat4,glm::vec3> get_MVP_Matrix();
        
        Plane(
        const std::string& modelObjFile, const std::string &texFile,
        float camera_behind_distant,float camera_up_distance, 
        float camera_ViewPoint_distance,float scaling_factor,
        float speed, float starting_height,
        const std::vector<float> &rotation_angles,
        const std::vector<std::string> &rotation_axises,
        const std::string &texture_UniformName = "plane_texture",
        const std::string& vertexFile = "shaders/plane/vertex.glsl", 
        const std::string& fragFile = "shaders/plane/fragment.glsl"
         );
    private:
        VertexArray vao;
        VertexBuffer* vbo;
        IndexBuffer* ibo;
        Shader shader;
        Texture tex;
        const int texture_BindSlot = 0;
        float scaling_factor;
        glm::vec3 position;
        glm::vec3 Up = glm::vec3(0,1,0);
        float yay_degree = 0;
        float pitch_degree = 0;
        float roll_degree = 0;
        std::string texture_UniformName;
        float speed = 0.10f;
        const float rotation_angle_per_frame_deg = 1.5f;
        float camera_behind_distant;
        float camera_up_distance;
        float camera_ViewPoint_distance;

};

