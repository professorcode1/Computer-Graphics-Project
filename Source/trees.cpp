#include "trees.h"

TreeSpecie::TreeSpecie(const std::string &assetFile, const VertexBufferLayout &vertex_layout)
:name(assetFile)
{
    std::vector<vertex_t> vertices_plane;
	std::vector<unsigned int> index_buffer_plane; 
	parse_complex_wavefront(assetFile, vertices_plane, index_buffer_plane);
	this->vbo = new VertexBuffer(vertices_plane.data(), vertices_plane.size() * sizeof(vertex_t));
	this->ibo = new IndexBuffer(index_buffer_plane.data(), index_buffer_plane.size());
	this->vao.AddBuffer(*vbo, vertex_layout);
    this->vao.AddElementBuffer(*ibo);
    this->vao.Unbind();
    this->vbo->Unbind();
    this->ibo->Unbind();
}
TreeSpecie::TreeSpecie(TreeSpecie &&other) noexcept :
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

TreeSpecie::~TreeSpecie(){
}
void TreeSpecie::render() const {
    vao.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ibo->GetCount() , GL_UNSIGNED_INT, nullptr));
    this->vao.Unbind();
    this->ibo->Unbind();
    this->vbo->Unbind();
}

Tree::Tree(
    const glm::vec3 &position, const glm::vec3 &normal, 
    const bool rotate_to_normal,
    const float scaling_factor,
    TreeSpecie const * const specie, const uint32_t texture_slot
):
    specie_m{specie},
    texture_slot_m{texture_slot}
{
    this->model_matrix_m = glm::scale(
        glm::translate(glm::mat4(1.0f), position), 
        glm::vec3(scaling_factor, scaling_factor, scaling_factor)
        );

    this->model_matrix_m = glm::rotate(this->model_matrix_m, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
    if(rotate_to_normal){
        const glm::vec3 rotation_vector = glm::cross(normal, glm::vec3(0.0, 1.0, 0.0));
        const float rotation_angle = glm::acos(normal.y / glm::length(normal));
        this->model_matrix_m = glm::rotate(this->model_matrix_m, rotation_angle, rotation_vector);
    }
}

void Tree::render()const {
    // std::cout<<"calling specie_m to render"<<std::endl;
    this->specie_m->render();
}

Trees::Trees(
    unsigned int Trees_per_division,
    const int tree_scale,
    const bool align_with_normal,
    const Terrain &terrain,
    const glm::vec3 &sun_dir,
    const float fog_density,
    const std::string &tree_assets_folder, 
    const std::string &tree_texture_folders,
    const std::string &height_extract_file,
    const std::string &vertex_shader_file,
    const std::string &fragment_shader_file
):
    shader(vertex_shader_file, fragment_shader_file),
    height_extractor(height_extract_file),
    sun_dir_m{sun_dir},
    fog_density_m{fog_density}
{
    VertexBufferLayout vertex_layout_simple;
	CREATE_VERTEX_LAYOUT(vertex_layout_simple);
    this->shader.Bind();
	this->shader.SetUniform3f("sun_direction_vector", sun_dir_m.x , sun_dir_m.y , sun_dir_m.z );
	this->shader.SetUniform1f("fog_density", fog_density_m);
    for (const auto & tree_asset_file : std::filesystem::directory_iterator(tree_assets_folder)){
        this->Species.emplace_back(std::move(TreeSpecie(tree_asset_file.path(), vertex_layout_simple)));
        // break;
    }
    for(const auto & tree_texture_file : std::filesystem::directory_iterator(tree_texture_folders)){
        this->textures.emplace_back(tree_texture_file.path());
        this->textures.back().Unbind();
        // break;
    }
    const int number_of_species = this->Species.size();
    uint32_t number_of_textures = this->textures.size();
    uint32_t binding_slot_start = 0;
    {
        float tree_per_division_f = static_cast<float>(Trees_per_division);
        int tree_per_division_sqrt = ceil(sqrt(tree_per_division_f));
        Trees_per_division = tree_per_division_sqrt * tree_per_division_sqrt;
        int divisions = terrain.get_divisions();
        glm::vec3 *tree_positions_cpu = new glm::vec3[ 2 * Trees_per_division ];
        int tree_per_divison_per_axis = divisions / tree_per_division_sqrt;
        // std::cout<<tree_per_divison_per_axis<<std::endl;
        for(int row_tree_index = 0; row_tree_index < tree_per_division_sqrt ; row_tree_index++){
            for(int col_tree_index = 0; col_tree_index < tree_per_division_sqrt ; col_tree_index++){
                float row = row_tree_index * tree_per_divison_per_axis + Randomness::Random_t::Random.rand( tree_per_divison_per_axis);
                float col = col_tree_index * tree_per_divison_per_axis + Randomness::Random_t::Random.rand( tree_per_divison_per_axis);
                // std::cout<<row<<" "<<col<<std::endl;
                int row_major_index = col_tree_index + row_tree_index * tree_per_division_sqrt;
                tree_positions_cpu[ 2 * row_major_index ] = glm::vec3(row, 0, col);
            }
        }
        unsigned int tree_positions_gpu;
        GLCall(glCreateBuffers(1, &tree_positions_gpu));
        GLCall(glNamedBufferData(tree_positions_gpu, Trees_per_division * 2 * sizeof(glm::vec3) , tree_positions_cpu, GL_STATIC_DRAW));
        height_extractor.Bind();
        height_extractor.SetUniform1i("number_of_divs", divisions);
        height_extractor.SetUniform1i("number_of_trees_sqrt", tree_per_division_sqrt);
        height_extractor.bindSSOBuffer(0, terrain.get_terrain_ssbo_buffer_id());
        height_extractor.bindSSOBuffer(1, tree_positions_gpu);
        height_extractor.launch_and_Sync( ceil((float)tree_per_division_sqrt/8), ceil((float)tree_per_division_sqrt/8) , 1);
        GLCall(glGetNamedBufferSubData(tree_positions_gpu, 0, Trees_per_division * 2 * sizeof(glm::vec3) , tree_positions_cpu));
        GLCall(glDeleteBuffers(1, &tree_positions_gpu));
        
        for(uint32_t tree_index=0; tree_index<Trees_per_division ; tree_index++){
            int specie_index = Randomness::Random_t::Random.rand(number_of_species);
            uint32_t texture_index = binding_slot_start + Randomness::Random_t::Random.rand(number_of_textures);
            this->trees.emplace_back(
                tree_positions_cpu[ 2 * tree_index ], 
                tree_positions_cpu[ 2 * tree_index + 1 ], 
                align_with_normal,
                tree_scale, 
                &this->Species.at(specie_index), 
                texture_index
                );
            if(false){
                std::cout <<
                tree_positions_cpu[ 2 * tree_index ].x << " "<<
                tree_positions_cpu[ 2 * tree_index ].y << " "<<
                tree_positions_cpu[ 2 * tree_index ].z << " "<<
                std::endl;
            }
        }
        delete[] tree_positions_cpu;
    }

}

void Trees::render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos){
    shader.Bind();
    shader.SetUniform1i("tree_tex_0", 0);
    shader.SetUniform1i("tree_tex_1", 1);
    shader.SetUniform1i("tree_tex_2", 2);
    shader.SetUniform1i("tree_tex_3", 3);
    for(uint32_t texture_index=0; texture_index<this->textures.size() ; texture_index++){
        this->textures[texture_index].Bind(texture_index);
    }

	shader.SetUniform3f("camera_loc", camera_pos.x, camera_pos.y, camera_pos.z);

    for(const auto &tree:this->trees){
        shader.SetUniformMat4f("VPMatrix", ViewProjection );
        shader.SetUniformMat4f("ModelMatrix", tree.model_matrix_m );
        shader.SetUniform1i("texture_to_use", tree.texture_slot_m);
        tree.render();
    }

    for(uint32_t texture_index=0; texture_index<this->textures.size() ; texture_index++){
        this->textures[texture_index].Unbind();
    }
}