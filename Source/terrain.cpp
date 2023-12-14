#include "terrain.h"
#include "waveFrontFileApi.h"

TerrainPatch::TerrainPatch( 
	const std::string &terrainTextureFile,float fog_density , 
    const std::vector<int>  &ActiveWaveNumber, float rotation_angle_fractal_ground,
	float output_increase_fctr_, float input_shrink_fctr_, 
    float lacunarity, float persistance,
	bool writeToFile,int div, 
	float length_of_side, 
	float Mountain_Scale_Factor, 
    const glm::vec3 &sun_direction,
	glm::vec2 terrain_index,
    const std::string &terrainGeneratorShaderFile, const std::string &vertexShaderFile, 
	const std::string &fragmentShaderFile
	):
	terrain_generator(terrainGeneratorShaderFile), 
	shader(vertexShaderFile, fragmentShaderFile),
	tex(terrainTextureFile),
	div_m{div},
	length_of_side_m{length_of_side},
	mountain_scale_factor_m{Mountain_Scale_Factor},
	sun_dir_m{sun_direction},
	terrain_index_m{terrain_index}
	{
    VertexBufferLayout vertex_layout_simple;
	CREATE_VERTEX_LAYOUT(vertex_layout_simple);

    
	GLCall(glCreateBuffers(1, &this->VBO));
	GLCall(glCreateBuffers(1, &this->EBO));

    GLCall(glNamedBufferData(this->VBO, ( div + 1 ) * ( div + 1 ) * sizeof(struct vertex_t) , NULL, GL_STATIC_DRAW));
	GLCall(glNamedBufferData(this->EBO, div * div * 6 * 4, NULL, GL_STATIC_DRAW));

	this->vertex_buffer = new VertexBuffer(this->VBO);
	this->index_buffer = new IndexBuffer(this->EBO, div * div * 6);
	this->shader.Bind();
	// Texture tex(terrainTextureFile);
	this->shader.SetUniform1i("mountain_tex", 0); 
	this->shader.SetUniform1f("fog_density", fog_density); 
	this->shader.SetUniform1i("mountain_tex", 0);
	this->shader.SetUniform3f("sun_direction_vector", sun_dir_m.x , sun_dir_m.y , sun_dir_m.z );

	this->terrain_generator.Bind();
	this->terrain_generator.SetUniform1i("number_of_divs", div);
	this->terrain_generator.SetUniform1f("length_of_side", length_of_side_m);
	int ActiveWaveFreqsGround = 0;
    for(int freq : ActiveWaveNumber){
        ActiveWaveFreqsGround |= (1 << freq);
	}
	// std::cout<<"ActiveWaveFreqsGround\t"<<ActiveWaveFreqsGround<<std::endl;

	this->terrain_generator.SetUniform1i("ActiveWaveFreqsGround", ActiveWaveFreqsGround);
	this->terrain_generator.SetUniform1f("rotation_Angle", M_PI * rotation_angle_fractal_ground / 180.0f);
	this->terrain_generator.SetUniform1f("output_increase_fctr", output_increase_fctr_);
	this->terrain_generator.SetUniform1f("input_shrink_fctr", input_shrink_fctr_);
	this->terrain_generator.SetUniform1f("lacunarity", lacunarity);
	this->terrain_generator.SetUniform1f("persistance", persistance);
	this->terrain_generator.SetUniform1f("length_of_side", length_of_side_m);
	this->terrain_generator.SetUniform2f("terrain_index", terrain_index_m.x,terrain_index_m.y );
    glm::mat4 mountain_model;
	mountain_model = glm::scale(glm::mat4(1.0f), glm::vec3(Mountain_Scale_Factor, Mountain_Scale_Factor, Mountain_Scale_Factor));
	terrain_generator.SetUniformMat4f("MountainModelMatrix", mountain_model);
	this->terrain_generator.bindSSOBuffer(0, vertex_buffer->GetRenderedID());
	this->terrain_generator.bindSSOBuffer(1, index_buffer->GetRenderedID());
	this->terrain_generator.launch_and_Sync(ceil((float)(div +1)/8), ceil((float)(div +1)/8), 1);
	this->vertex_array.AddBuffer(*vertex_buffer, vertex_layout_simple);
	this->vertex_array.AddElementBuffer(*index_buffer);



	if(writeToFile)
		write_to_file(this->VBO,this->EBO,div);
	// std::cout<<min_x<<" "<< max_x<<" "<<  min_z<<" "<<  max_z<<std::endl;

	this->tex.Unbind();
	this->vertex_array.Unbind();
	this->vertex_buffer->Unbind();
	this->index_buffer->Unbind();
	this->shader.Unbind();
}

TerrainPatch::~TerrainPatch(){
	// glDeleteBuffers(1, &this->VBO);
	// glDeleteBuffers(1, &this->EBO);	
	delete this->index_buffer;
	delete this->vertex_buffer;

}

void TerrainPatch::render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos){
		this->shader.Bind();
		this->shader.SetUniformMat4f("ViewProjectionMatrix", ViewProjection   );
		this->shader.SetUniform3f("camera_loc", camera_pos.x, camera_pos.y, camera_pos.z);
		this->vertex_array.Bind();

		this->tex.Bind();
		GLCall(glDrawElements(GL_TRIANGLES, this->index_buffer->GetCount() , GL_UNSIGNED_INT, nullptr));

		this->tex.Unbind();
		this->vertex_array.Unbind();
		this->vertex_buffer->Unbind();
		this->index_buffer->Unbind();
		this->shader.Unbind();
}	


int TerrainPatch::get_divisions() const{
	return div_m;
}

unsigned int TerrainPatch::get_terrain_ssbo_buffer_id() const{
	return this->VBO;
}

float TerrainPatch::get_Mountain_Scale() const{
	return this->mountain_scale_factor_m;
}

std::tuple<float, float, float,float> TerrainPatch::get_corners() const {
	return std::make_tuple(
		(this->terrain_index_m.x - 0.5) * this->length_of_side_m * this->mountain_scale_factor_m,
		(this->terrain_index_m.x + 0.5) * this->length_of_side_m * this->mountain_scale_factor_m,
		(this->terrain_index_m.y - 0.5) * this->length_of_side_m * this->mountain_scale_factor_m,
		(this->terrain_index_m.y + 0.5) * this->length_of_side_m * this->mountain_scale_factor_m
		);
}

static TerrainPatch* terrain_patch_generator(
	const float fog_densty, const glm::vec3 &sun_direction, 
	const nlohmann::json &terrainParam, const glm::vec2 &center 
){
	return new TerrainPatch(
		terrainParam.at("noise texture file"), fog_densty,terrainParam["wave numbers active"], 
		terrainParam.at("rotation angle fractal ground"), terrainParam.at("output_increase_fctr_"), terrainParam.at("input_shrink_fctr_"), 
		terrainParam.at("lacunarity"), terrainParam.at("persistance"),terrainParam.at("write to file"), terrainParam.at("divisions"), 
		terrainParam.at("length of one side"), 
		terrainParam.at("Mountain Scale Factor"), sun_direction, center
		);
}


Terrain::Terrain(
	const float fog_densty, const glm::vec3 &sun_direction, const nlohmann::json &terrainParam
):current_center(0.0, 0.0){
	this->grid[0][0] = terrain_patch_generator(fog_densty, sun_direction, terrainParam, glm::vec2(-1.0, -1.0));
	this->grid[0][1] = terrain_patch_generator(fog_densty, sun_direction, terrainParam, glm::vec2(-1.0,  0.0));
	this->grid[0][2] = terrain_patch_generator(fog_densty, sun_direction, terrainParam, glm::vec2(-1.0,  1.0));
	this->grid[1][0] = terrain_patch_generator(fog_densty, sun_direction, terrainParam, glm::vec2( 0.0, -1.0));
	this->grid[1][1] = terrain_patch_generator(fog_densty, sun_direction, terrainParam, glm::vec2( 0.0,  0.0));
	this->grid[1][2] = terrain_patch_generator(fog_densty, sun_direction, terrainParam, glm::vec2( 0.0,  1.0));
	this->grid[2][0] = terrain_patch_generator(fog_densty, sun_direction, terrainParam, glm::vec2( 1.0, -1.0));
	this->grid[2][1] = terrain_patch_generator(fog_densty, sun_direction, terrainParam, glm::vec2( 1.0,  0.0));
	this->grid[2][2] = terrain_patch_generator(fog_densty, sun_direction, terrainParam, glm::vec2( 1.0,  1.0));

}

void Terrain::render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos){
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			this->grid[i][j]->render(ViewProjection, camera_pos);
		}
	}
}