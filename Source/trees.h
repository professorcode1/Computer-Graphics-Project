#define GLM_ENABLE_EXPERIMENTAL

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
#include "unordered_set"
#include "../IncGLM.hpp"
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
    Texture* tex;
    void render()const ;

public:
    TreeSpecie(const std::string &assetFile, const VertexBufferLayout &vertex_layout_simple, const std::string tex_file);
    TreeSpecie(TreeSpecie &&other)noexcept;
    ~TreeSpecie();
    
    friend class Trees;
};


class Trees{
private:
    static std::vector<TreeSpecie> Species;
    
    Shader shader;
    const glm::vec3 sun_dir_m;
    const float fog_density_m;
    uint32_t Trees_per_division_m;
    uint32_t tree_positions_gpu;
    uint32_t tree_scale_m;
    glm::vec3 *tree_positions_cpu;
public:
    Trees(
        const unsigned int Trees_per_division,
        const int tree_scale,
        const TerrainPatch &terrain,
        const glm::vec3 &sun_dir,
        const float fog_density,
        const std::string &vertex_shader_file = "shaders/trees/vertex.glsl",
        const std::string &fragment_shader_file = "shaders/trees/fragment.glsl"
    );
    void render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos);
    void populateSpecies(const std::string &tree_assets_folder = "assets/Trees");
};