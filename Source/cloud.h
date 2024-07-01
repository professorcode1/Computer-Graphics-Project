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
#include "../FOSS_Code/PerlinNoise.hpp"
#include <list>
/*
TODO: BATCH, and frustum cull
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
    glm::vec3 position_m;
    float rotation_m;
    float scaling_factor_m;
    void render()const ;

    glm::mat4 get_model_matrix() const ;

    void flow(
        const siv::PerlinNoise &perlin, 
        float input_shrink_factor, 
        float time_shrink_factor,
        float velocity,
        float angular_velocty,
        float time
        );

public:
    Cloud(
        const glm::vec3 &position,
        const float scaling_factor,
        const float rotation,
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
    const siv::PerlinNoise perlin;
    const float input_shrink_factor_m;
    const float velocity_m;
    const float angular_velocty_m;
    const float time_shrink_factor_m;
public:
    Clouds(
        const unsigned int clouds_per_divison,
        const float cloud_scale,
        const std::tuple<float, float, float,float> &terrain_corners,
        const glm::vec3 &sun_dir,
        const float height_to_start,
        const float input_shrink_factor,
        const float time_shrink_factor,
        const float velocity,
        const float angular_velocty,
        const std::string &cloud_assets_folder = "assets/cloud", 
        const std::string &vertex_shader_file = "shaders/clouds/vertex.glsl",
        const std::string &fragment_shader_file = "shaders/clouds/fragment.glsl"
    );
    void render(const glm::mat4 &ViewProjection) ;

    void flow(
            int time
        );
};