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
    VertexArray  vao;
    VertexBuffer* vbo;
    IndexBuffer* ibo;
    std::string name;
    void render()const ;

public:
    TreeSpecie(const std::string &assetFile, const VertexBufferLayout &vertex_layout_simple);
    TreeSpecie(TreeSpecie &&other)noexcept;
    ~TreeSpecie();
    
    friend class Tree;
};

class Tree{
private:
    TreeSpecie const * const specie_m;
    const uint32_t texture_slot_m;
    glm::mat4 model_matrix_m;

    void render()const ;

public:
    Tree(
        const glm::vec3 &position,const glm::vec3 &normal,
        const bool rotate_to_normal, const float scaling_factor,
        TreeSpecie const * const specie, const uint32_t texture_slot
        );

    friend class Trees;
};

class Trees{
private:
    std::vector<Texture> textures;
    std::vector<TreeSpecie> Species;
    std::list<Tree> trees;
    Shader shader;
    ComputeShader height_extractor;
    const glm::vec3 sun_dir_m;
    const float fog_density_m;
public:
    Trees(
        const unsigned int Trees_per_division,
        const int tree_scale,
        const bool align_with_normal,
        const Terrain &terain,
        const glm::vec3 &sun_dir,
        const float fog_density,
        const std::string &tree_assets_folder = "assets/Trees/Models Obj", 
        const std::string &tree_texture_folders = "assets/Trees/Textures",
        const std::string &height_extract_file = "shaders/trees/extract_height.glsl",
        const std::string &vertex_shader_file = "shaders/trees/vertex.glsl",
        const std::string &fragment_shader_file = "shaders/trees/fragment.glsl"
    );
    void render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos);
};