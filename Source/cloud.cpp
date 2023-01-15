#include "cloud.h"

CloudType::CloudType(const std::string &assetFile, const VertexBufferLayout &vertex_layout_simple){
	std::vector<simple_vertex_t> vertices_plane;
	std::vector<unsigned int> index_buffer_plane; 
	parse_complex_wavefront(assetFile, vertices_plane, index_buffer_plane);
	this->vbo = new VertexBuffer(vertices_plane.data(), vertices_plane.size() * sizeof(simple_vertex_t));
	this->ibo = new IndexBuffer(index_buffer_plane.data(), index_buffer_plane.size());
    // std::cout<<"vertex buffer size "<<vertices_plane.size()<<std::endl;
    // std::cout<<"index buffer size "<<index_buffer_plane.size()<<std::endl;
	this->vao.AddBuffer(*vbo, vertex_layout_simple);
    this->vao.AddElementBuffer(*ibo);
    this->vao.Unbind();
    this->vbo->Unbind();
    this->ibo->Unbind();
}

CloudType::CloudType(CloudType &&other) noexcept :
    vao{std::move(other.vao)},
    name{std::move(other.name)}
{
    if(&other == this){
        return ;
    }
    this->vbo = other.vbo;
    this->ibo = other.ibo;
    other.vbo = nullptr;
    other.ibo = nullptr;
}

CloudType::~CloudType(){}; //can write this but it exists the lifespan of the program so not really necessary

void CloudType::render() const {
    vao.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ibo->GetCount() , GL_UNSIGNED_INT, nullptr));
    this->vao.Unbind();
    this->ibo->Unbind();
    this->vbo->Unbind();
}

Cloud::Cloud(
    const glm::vec3 &position,
    const float scaling_factor,
    CloudType const * const specie
):specie_m{specie}{
    // std::cout<<"position "<<position.x<<" "<<position.y<<" "<<position.z<<std::endl;
	// std::cout<<"scale "<<scaling_factor<<std::endl;
    this->model_matrix_m = glm::scale(
        glm::translate(glm::mat4(1.0f), position), 
        glm::vec3(scaling_factor, scaling_factor, scaling_factor)
    );
}

void Cloud::render() const {
	this->specie_m->render();
}

Clouds::Clouds(
    unsigned int clouds_per_divison,
    const float cloud_scale,
    const Terrain &terrain,
    const glm::vec3 &sun_dir,
    const float height_to_start,
    const std::string &cloud_assets_folder, 
    const std::string &vertex_shader_file,
    const std::string &fragment_shader_file
):
shader( vertex_shader_file, fragment_shader_file ),
sun_dir_m{sun_dir}
{
	VertexBufferLayout vertex_layout_simple;
	CREATE_SIMPLE_VERTEX_LAYOUT(vertex_layout_simple);
    this->shader.Bind();
	this->shader.SetUniform3f("sun_direction_vector", sun_dir_m.x , sun_dir_m.y , sun_dir_m.z );
    for (const auto & tree_asset_file : std::filesystem::directory_iterator(cloud_assets_folder)){
        this->cloud_type.emplace_back(tree_asset_file.path(), vertex_layout_simple);
        // break;
    }
    const int number_of_cloud_types = this->cloud_type.size();
	float min_x, min_z, max_x, max_z;
	std::tie(min_x, max_x, min_z, max_z) = terrain.get_corners();



    float clouds_per_division_f = static_cast<float>(clouds_per_divison);
    int clouds_per_division_sqrt = ceil(sqrt(clouds_per_division_f));
    clouds_per_divison = clouds_per_division_sqrt * clouds_per_division_sqrt;
    const float dist_per_div_sqrt = (max_z - min_z ) / clouds_per_division_sqrt;
    for(int row_cloud_index = 0; row_cloud_index < clouds_per_division_sqrt ; row_cloud_index++ ){
        for(int col_cloud_index = 0 ; col_cloud_index < clouds_per_division_sqrt ; col_cloud_index++ ){
		float x = min_x + row_cloud_index * dist_per_div_sqrt + Randomness::Random_t::Random.rand_f(dist_per_div_sqrt);
		float z = min_z + col_cloud_index * dist_per_div_sqrt + Randomness::Random_t::Random.rand_f(dist_per_div_sqrt);
		int cloud_type = Randomness::Random_t::Random.rand( this->cloud_type.size() );
		clouds.emplace_back( glm::vec3(x,height_to_start,z), cloud_scale , &this->cloud_type.at(cloud_type) );
        }
    }
}

void Clouds::render(const glm::mat4 &ViewProjection)  {
    shader.Bind();
	shader.SetUniform3f("sun_direction_vector", sun_dir_m.x, sun_dir_m.y, sun_dir_m.z);
    for(const auto &cloud:this->clouds){
        shader.SetUniformMat4f("MVPMatrix", ViewProjection * cloud.model_matrix_m );
        cloud.render();
    }
}