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
TODO: BATCH
*/
class CloudType{
private:
    VertexArray  vao;
    VertexBuffer* vbo;
    IndexBuffer* ibo;
    std::string name;
    void render()const ;

public:
    CloudType(const std::string &assetFile, const VertexBufferLayout &vertex_layout_simple);
    CloudType(CloudType &&other)noexcept;
    ~CloudType();
    
    friend class Cloud;
};

class Cloud{
private:
    CloudType const * const specie_m;
    glm::mat4 model_matrix_m;

    void render()const ;

public:
    Cloud(
        const glm::vec3 &position,
        const float scaling_factor,
        CloudType const * const specie
        );

    friend class Clouds;
};

class Clouds{
private:
    std::vector<CloudType> cloud_type;
    std::list<Cloud> clouds;
    Shader shader;
    const glm::vec3 sun_dir_m;
public:
    Clouds(
        const unsigned int clouds_per_divison,
        const float cloud_scale,
        const Terrain &terrain,
        const glm::vec3 &sun_dir,
        const float height_to_start,
        const std::string &cloud_assets_folder = "assets/cloud", 
        const std::string &vertex_shader_file = "shaders/clouds/vertex.glsl",
        const std::string &fragment_shader_file = "shaders/clouds/fragment.glsl"
    );
    void render(const glm::mat4 &ViewProjection) ;
};