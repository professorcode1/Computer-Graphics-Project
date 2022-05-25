#include "Cherno_OpenGL_Library/IndexBuffer.h"
#include "Cherno_OpenGL_Library/Shader.h"
#include "Cherno_OpenGL_Library/VertexArray.h"
#include "Cherno_OpenGL_Library/VertexBuffer.h"
#include "Cherno_OpenGL_Library/Texture.h"
#include "Cherno_OpenGL_Library/VertexBufferLayout.h"
#include "vertex_index_layout.h"
#include <cstddef>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include<GLFW/glfw3.h>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include<glm/gtx/rotate_vector.hpp>

void string_split( std::string &line, std::vector<std::string> &split, std::string delimiter );
void parse_simple_wavefront(const std::string& filename, std::vector<vertex_t> &vertices, std::vector< unsigned int> &index_buffer);

class Plane{
    public:
        void set_scale(glm::vec3 scale){
            this->scale = scale;
        }

        void set_position(glm::vec3 position){
            this->position = position;
        }
        void set_Speed(float speed){
            this->speed = speed;
        }

        void render(glm::mat4 viewAndProjection);

        void catchInputs(GLFWwindow* window);

        Plane(const std::string& computeFile, const std::string& vertexFile, const std::string& fragFile, const std::string& modelObjFile,
         const std::string &texFile, int binding_Pnt, const VertexBufferLayout &vertex_layout, const std::string MVP_uniform_name = "MVP_plane", 
         const std::string &texture_UniformName = "plane_texture");
    private:
        VertexArray vao;
        VertexBuffer*  vbo;
        IndexBuffer* ibo;
        ComputeShader collition_detection;
        Shader shader;
        Texture tex;
        int texture_BindSlot;
        glm::vec3 scale = glm::vec3(0.8, 0.8, 0.8);
        glm::vec3 position = glm::vec3(3,3,3);
        float yay_degree = 0;
        float pitch_degree = 0;
        std::string MVP_uniform_name;
        std::string texture_UniformName;
        float speed = 0.05f;
        float rotation_angle_per_frame_deg = 3.0f;

};

