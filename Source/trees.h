#include "../OpenGL/IndexBuffer.h"
#include "../OpenGL/Shader.h"
#include "../OpenGL/VertexArray.h"
#include "../OpenGL/VertexBuffer.h"
#include "../OpenGL/Texture.h"
#include "../OpenGL/VertexBufferLayout.h"
#include "terrain.h"
#include "waveFrontFileApi.h"
#include <filesystem>
#include "random.hpp"
/*
CAN AND SHOULD BE BATCHED
TODO: BATCH WHENEVER YOU CAN
for now I'm gonna just make many draw calls
any gpu with core opengl 4.3 and above will be overpowered for this application anyways
*/
class TreeSpecie{
private:
    VertexArray vao;
    VertexBuffer*  vbo;
    IndexBuffer* ibo;

public:
    TreeSpecie(const std::string &assetFile, const VertexBufferLayout &vertex_layout_simple);
    
    
    friend class Trees;
};

class Tree{
private:
    TreeSpecie const * const specie_m;
    const int texture_BindSlot_m;
    glm::mat4 model_matrix_m;

public:
    Tree(const glm::vec3 &position, const float scaling_factor,TreeSpecie* specie, int texture_BindSlot);

    friend class Trees;
};

class Trees{
private:
    std::list<Texture> textures;
    std::vector<TreeSpecie> Species;
    std::list<Tree> trees;
    Shader shader;
    ComputeShader height_extractor;
public:
    Trees(
        const unsigned int Trees_per_division,
        const int tree_scale,
        const Terrain &terain,
        const std::string &tree_assets_folder = "assets/Trees/Models Obj", 
        const std::string &tree_texture_folders = "assets/Trees/Textures",
        const std::string &height_extract_file = "shaders/trees/extract_height.glsl",
        const std::string &vertex_shader_file = "shaders/trees/vertex.glsl",
        const std::string &fragment_shader_file = "shaders/trees/fragment.glsl"
    );
    void render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos);
};