#include "trees.h"

TreeSpecie::TreeSpecie(const std::string &assetFile, const VertexBufferLayout &vertex_layout){
    std::vector<vertex_t> vertices_plane;
	std::vector<unsigned int> index_buffer_plane; 
	parse_complex_wavefront(assetFile, vertices_plane, index_buffer_plane);
	this->vbo = new VertexBuffer(vertices_plane.data(), vertices_plane.size() * sizeof(vertex_t));
	this->ibo = new IndexBuffer(index_buffer_plane.data(), index_buffer_plane.size());
	this->vao.AddBuffer(*vbo, vertex_layout);
}


Tree::Tree(
    const glm::vec3 &position, const float scaling_factor,
    TreeSpecie* specie, int texture_BindSlot
):
    specie_m{specie},
    texture_BindSlot_m{texture_BindSlot}
{
    this->model_matrix_m = glm::scale(
        glm::translate(glm::mat4(1.0f), position), 
        glm::vec3(scaling_factor, scaling_factor, scaling_factor)
        );
}


Trees::Trees(
    unsigned int Trees_per_division,
    const int tree_scale,
    const Terrain &terain,
    const std::string &tree_assets_folder, 
    const std::string &tree_texture_folders,
    const std::string &height_extract_file,
    const std::string &vertex_shader_file,
    const std::string &fragment_shader_file
):
    shader(vertex_shader_file, fragment_shader_file),
    height_extractor(height_extract_file)
{
    std::cout<<vertex_shader_file<<"\t"<<fragment_shader_file<<std::endl;
    VertexBufferLayout vertex_layout_simple;
	CREATE_SIMPLE_VERTEX_LAYOUT(vertex_layout_simple);
    for (const auto & tree_asset_file : std::filesystem::directory_iterator(tree_assets_folder)){
        this->Species.emplace_back(tree_asset_file.path(), vertex_layout_simple);
    }
    for(const auto & tree_texture_file : std::filesystem::directory_iterator(tree_texture_folders)){
        this->textures.emplace_back(tree_texture_file.path());
    }
    const int number_of_species = this->Species.size();
    uint32_t number_of_textures = this->textures.size();
    uint32_t binding_slot_start = 0;
    {
        float tree_per_division_f = static_cast<float>(Trees_per_division);
        int tree_per_division_sqrt = ceil(sqrt(tree_per_division_f));
        Trees_per_division = tree_per_division_sqrt * tree_per_division_sqrt;
        float x_min, z_min;
        float x_max, z_max;
        std::tie(x_min, x_max, z_min, z_max) = terain.get_dimentions();
        int divisions = terain.get_divisions();
        float x_dim = x_max - x_min;
        float z_dim = z_max - z_min;
        float x_stride = x_dim / tree_per_division_sqrt;
        float z_stride = z_dim / tree_per_division_sqrt;
        glm::vec3 *tree_positions_cpu = new glm::vec3[Trees_per_division];
        for(int row_tree_index = 0; row_tree_index < tree_per_division_sqrt ; row_tree_index++){
            for(int col_tree_index = 0; col_tree_index < tree_per_division_sqrt ; col_tree_index++){
                float x_loc = x_min + x_stride * (row_tree_index + Randomness::Random_t::Random.rand());
                float z_loc = z_min + z_stride * (col_tree_index + Randomness::Random_t::Random.rand());
                int row_major_index = col_tree_index + row_tree_index * tree_per_division_sqrt;
                tree_positions_cpu[row_major_index] = glm::vec3(x_loc, 0, z_loc);
            }
        }
        unsigned int tree_positions_gpu;
        glCreateBuffers(1, &tree_positions_gpu);
        glNamedBufferData(tree_positions_gpu, Trees_per_division * sizeof(glm::vec3) , tree_positions_cpu, GL_STATIC_DRAW);
        height_extractor.Bind();
        height_extractor.SetUniform1f("min_x", x_min);
        height_extractor.SetUniform1f("max_x", x_max);
        height_extractor.SetUniform1f("min_z", z_min);
        height_extractor.SetUniform1f("max_z", z_max);
        height_extractor.SetUniform1i("number_of_divs", divisions);
        height_extractor.SetUniform1i("number_of_trees_sqrt", tree_per_division_sqrt);
        height_extractor.bindSSOBuffer(0, terain.get_terrain_ssbo_buffer_id());
        height_extractor.bindSSOBuffer(1, tree_positions_gpu);
        height_extractor.launch_and_Sync( ceil((float)tree_per_division_sqrt/8), ceil((float)tree_per_division_sqrt/8) , 1);
        glGetNamedBufferSubData(tree_positions_gpu, 0, Trees_per_division * sizeof(glm::vec3) , tree_positions_cpu);
        glDeleteBuffers(1, &tree_positions_gpu);
        
        for(int tree_index=0; tree_index<Trees_per_division ; tree_index++){
            int specie_index = Randomness::Random_t::Random.rand(number_of_species);
            uint32_t texture_slot = binding_slot_start + Randomness::Random_t::Random.rand(number_of_textures);
            this->trees.emplace_back(tree_positions_cpu[tree_index], tree_scale, &this->Species.at(specie_index), texture_slot);
        }
        delete[] tree_positions_cpu;
    }

}

void Trees::render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos){

}