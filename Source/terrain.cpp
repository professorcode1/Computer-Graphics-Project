#include "terrain.h"
#include "waveFrontFileApi.h"

static void setup_the_terrain_generator_compute_shader(
	ComputeShader &terrain_generator,
	const std::vector<int>  &ActiveWaveNumber, float rotation_angle_fractal_ground,
	float output_increase_fctr_, float input_shrink_fctr_, 
    float lacunarity, float persistance,
	int div, 
	float length_of_side, 
	float Mountain_Scale_Factor,
	glm::vec2 &terrain_index,
	uint32_t vertex_buffer_id,
	uint32_t index_buffer_id
){
	terrain_generator.Bind();
	terrain_generator.SetUniform1i("number_of_divs", div);
	int ActiveWaveFreqsGround = 0;
    for(int freq : ActiveWaveNumber){
        ActiveWaveFreqsGround |= (1 << freq);
	}
	// std::cout<<"ActiveWaveFreqsGround\t"<<ActiveWaveFreqsGround<<std::endl;

	terrain_generator.SetUniform1i("ActiveWaveFreqsGround", ActiveWaveFreqsGround);
	terrain_generator.SetUniform1f("rotation_Angle", M_PI * rotation_angle_fractal_ground / 180.0f);
	terrain_generator.SetUniform1f("output_increase_fctr", output_increase_fctr_);
	terrain_generator.SetUniform1f("input_shrink_fctr", input_shrink_fctr_);
	terrain_generator.SetUniform1f("lacunarity", lacunarity);
	terrain_generator.SetUniform1f("persistance", persistance);
	terrain_generator.SetUniform1f("length_of_side", length_of_side);
	terrain_generator.SetUniform2f("terrain_index", terrain_index.x,terrain_index.y );
    glm::mat4 mountain_model;
	mountain_model = glm::scale(glm::mat4(1.0f), glm::vec3(Mountain_Scale_Factor, Mountain_Scale_Factor, Mountain_Scale_Factor));
	terrain_generator.SetUniformMat4f("MountainModelMatrix", mountain_model);
	terrain_generator.bindSSOBuffer(0, vertex_buffer_id);
	terrain_generator.bindSSOBuffer(1, index_buffer_id);
}

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
    const bool async_generation,
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

    glm::mat4 mountain_model;
	mountain_model = glm::scale(glm::mat4(1.0f), glm::vec3(Mountain_Scale_Factor, Mountain_Scale_Factor, Mountain_Scale_Factor));

	setup_the_terrain_generator_compute_shader(
		terrain_generator,ActiveWaveNumber,
		rotation_angle_fractal_ground,output_increase_fctr_,
		input_shrink_fctr_, lacunarity,
		persistance,div, length_of_side,Mountain_Scale_Factor, 
		terrain_index, vertex_buffer->GetRenderedID(),
		index_buffer->GetRenderedID()
	);

	if(async_generation){
		this->terrain_generator.launch(ceil((float)(div +1)/8), ceil((float)(div +1)/8), 1);
	}else{
		this->terrain_generator.launch_and_Sync(ceil((float)(div +1)/8), ceil((float)(div +1)/8), 1);
	}
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

void TerrainPatch::sync() {
	this->terrain_generator.sync();
}


void TerrainPatch::set_index(
    const glm::vec2 &new_index,const std::vector<int>  &ActiveWaveNumber, float rotation_angle_fractal_ground,
	float output_increase_fctr_, float input_shrink_fctr_, 
    float lacunarity, float persistance, 
	float length_of_side, 
	float Mountain_Scale_Factor
){
	if(new_index == this->terrain_index_m){
		return ;
	}
	this->terrain_index_m = new_index;
	setup_the_terrain_generator_compute_shader(
		terrain_generator,ActiveWaveNumber,
		rotation_angle_fractal_ground,output_increase_fctr_,
		input_shrink_fctr_, lacunarity,
		persistance,div_m, length_of_side,Mountain_Scale_Factor, 
		terrain_index_m, vertex_buffer->GetRenderedID(),
		index_buffer->GetRenderedID()
	);
	this->terrain_generator.launch_and_Sync(ceil((float)(div_m +1)/8), ceil((float)(div_m +1)/8), 1);
}