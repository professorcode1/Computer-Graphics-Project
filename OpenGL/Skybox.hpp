#include <string>
#include <vector>
#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "IndexBuffer.h"
#include "Texture.h"
class Skybox{
private:
    Texture *tex;
    Shader shader;
    VertexArray vao;
    VertexBuffer *vbo;  
    IndexBuffer *ibo;  
public:
    Skybox(
        const std::vector<std::string> &images_name = {
                "assets/skybox/right.jpg",
                "assets/skybox/left.jpg",
                "assets/skybox/top.jpg",
                "assets/skybox/bottom.jpg",
                "assets/skybox/front.jpg",
                "assets/skybox/back.jpg"
            },
        const std::string &vertex_shader_file = "shaders/skybox/vertex.glsl",
        const std::string &frgmnt_shader_file = "shaders/skybox/fragment.glsl"
        );    
    void render(glm::mat4 view, const glm::mat4 &projecton) ;
};