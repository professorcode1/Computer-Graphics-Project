#include "trees.h"

TreeSpecie::TreeSpecie(
    const std::string &assetFile, 
    const VertexBufferLayout &vertex_layout,
    const std::string tex_file
)
:name(assetFile)
{
    std::vector<vertex_t> vertices_plane;
	std::vector<unsigned int> index_buffer_plane; 
	parse_complex_wavefront(assetFile, vertices_plane, index_buffer_plane);
	this->vbo = new VertexBuffer(vertices_plane.data(), vertices_plane.size() * sizeof(vertex_t));
	this->ibo = new IndexBuffer(index_buffer_plane.data(), index_buffer_plane.size());
	this->tex = new Texture(tex_file);
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
    this->tex = other.tex;
    this->vbo = other.vbo;
    this->ibo = other.ibo;
    other.vbo = nullptr;
    other.ibo = nullptr;
    other.tex = nullptr;
}

TreeSpecie::~TreeSpecie(){
}
void TreeSpecie::render() const {
    vao.Bind();
    tex->Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ibo->GetCount() , GL_UNSIGNED_INT, nullptr));
    tex->Unbind();
    this->vao.Unbind();
    this->ibo->Unbind();
    this->vbo->Unbind();
}


std::vector<TreeSpecie> Trees::Species;
void Trees::populateSpecies(const std::string &tree_assets_folder){
    VertexBufferLayout vertex_layout_simple;
	CREATE_VERTEX_LAYOUT(vertex_layout_simple);
    for (const auto & tree_asset_folder : std::filesystem::directory_iterator(tree_assets_folder)){
        const auto obj_file = tree_asset_folder.path() / "model.obj";
        const auto texture_file = tree_asset_folder.path() / "texture.png";
        
        this->Species.emplace_back(
                    obj_file.string(), 
                    vertex_layout_simple,
                    texture_file.string()
        );
        // break;
    }
}

Trees::Trees(
    unsigned int Trees_per_division,
    const int tree_scale,
    const TerrainPatch&terrain,
    const glm::vec3 &sun_dir,
    const float fog_density,
    const std::string &height_extract_file,
    const std::string &vertex_shader_file,
    const std::string &fragment_shader_file
):
    shader(vertex_shader_file, fragment_shader_file),
    height_extractor(height_extract_file),
    sun_dir_m{sun_dir},
    fog_density_m{fog_density},
    tree_scale_m{tree_scale}
{

    this->shader.Bind();
	this->shader.SetUniform3f("sun_direction_vector", sun_dir_m.x , sun_dir_m.y , sun_dir_m.z );
	this->shader.SetUniform1f("fog_density", fog_density_m);
    this->shader.SetUniform1i("tree_tex_0", 0);
    if(this->Species.empty()){
        this->populateSpecies();
    }
    
    const int number_of_species = this->Species.size();
    {
        float tree_per_division_f = static_cast<float>(Trees_per_division);
        int tree_per_division_sqrt = ceil(sqrt(tree_per_division_f));
        Trees_per_division = tree_per_division_sqrt * tree_per_division_sqrt;
        int divisions = terrain.get_divisions();
        Trees_per_division_m = Trees_per_division;
        int tree_per_divison_per_axis = divisions / tree_per_division_sqrt;

        GLCall(glCreateBuffers(1, &tree_positions_gpu));
        GLCall(glNamedBufferData(tree_positions_gpu, Trees_per_division * sizeof(glm::vec3) , nullptr, GL_STATIC_DRAW));
        height_extractor.Bind();
        height_extractor.SetUniform1i("number_of_divs", divisions);
        height_extractor.SetUniform1i("number_of_trees_sqrt", tree_per_division_sqrt);
        height_extractor.SetUniform1ui("tree_per_divison_per_axis", tree_per_divison_per_axis);
        height_extractor.bindSSOBuffer(0, terrain.get_terrain_ssbo_buffer_id());
        height_extractor.bindSSOBuffer(1, tree_positions_gpu);
        height_extractor.launch_and_Sync( ceil((float)tree_per_division_sqrt/8), ceil((float)tree_per_division_sqrt/8) , 1);
        // glm::vec3 *tree_positions_cpu = new glm::vec3[ Trees_per_division_m ];
        // GLCall(glGetNamedBufferSubData(tree_positions_gpu, 0, Trees_per_division_m *  sizeof(glm::vec3) , tree_positions_cpu));
        // for(int i=0 ; i< Trees_per_division_m ; i++){
        //     std::cout<<glm::to_string(*(tree_positions_cpu + i))<<std::endl;
        // }
    }

    tree_ssbo_container = new Texture(tree_positions_gpu);

}

void Trees::render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos){
    shader.Bind();
    tree_ssbo_container->Bind(1, GL_TEXTURE_BUFFER);
    shader.SetUniformMat4f("VPMatrix", ViewProjection);
    shader.SetUniform1i("LocationArrayTexture", 1);
	shader.SetUniform3f("camera_loc", camera_pos.x, camera_pos.y, camera_pos.z);
    shader.SetUniform3f("sun_direction_vector", sun_dir_m.x, sun_dir_m.y, sun_dir_m.z);
    shader.SetUniform1f("scaling_factor", tree_scale_m);
     
    for(int tree_index = 0 ; tree_index < Trees_per_division_m ; tree_index++){
        shader.SetUniform1i("LocationArrayTextureIndex", tree_index);
        Species.at(0).render();
    }
    shader.Unbind();
}
void Trees::sync(){
    this->height_extractor.sync();
}