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

        void set_position(glm::vec3 position){
            this->position = position;
        }
        void set_Speed(float speed){
            this->speed = speed;
        }

        void render(glm::mat4 viewAndProjection);

        void catchInputs(GLFWwindow* window);
        
        std::tuple<glm::mat4,glm::vec3> get_MVP_Matrix(float FOVdeg, float nearPlane, float farPlane ,float aspect);
        
        Plane(const std::string& computeFile, const std::string& vertexFile, const std::string& fragFile, const std::string& modelObjFile,
         const std::string &texFile, int binding_Pnt,float camera_behind_distant,float camera_up_distance, float camera_ViewPoint_distance,float scaling_factor,float speed, const VertexBufferLayout &vertex_layout, 
         const std::string MVP_uniform_name = "MVP_plane", const std::string &texture_UniformName = "plane_texture");
    
    private:
        VertexArray vao;
        VertexBuffer*  vbo;
        IndexBuffer* ibo;
        ComputeShader collition_detection;
        Shader shader;
        Texture tex;
        int texture_BindSlot;
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

