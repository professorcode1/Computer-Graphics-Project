#include "terrain.h"
#include "waveFrontFileApi.h"

Terrain::Terrain( 
	const std::string &terrainTextureFile,float fog_density , 
    const std::vector<int>  &ActiveWaveNumber, float rotation_angle_fractal_ground,
	float output_increase_fctr_, float input_shrink_fctr_, 
    float lacunarity, float persistance,
	bool writeToFile,int div, 
	float min_x,float max_x,
	float min_z,float max_z, 
	float Mountain_Scale_Factor, 
    const glm::vec3 &sun_direction,
    const std::string &terrainGeneratorShaderFile, const std::string &vertexShaderFile, 
	const std::string &fragmentShaderFile
	):
	terrain_generator(terrainGeneratorShaderFile), 
	shader(vertexShaderFile, fragmentShaderFile),
	tex(terrainTextureFile),
	div_m{div},
	min_x_m{min_x},
	max_x_m{max_x},
	min_z_m{min_z},
	max_z_m{max_z},
	mountain_scale_factor_m{Mountain_Scale_Factor},
	sun_dir_m{sun_direction}
	{
    VertexBufferLayout vertex_layout_simple;
	CREATE_SIMPLE_VERTEX_LAYOUT(vertex_layout_simple);

    
	GLCall(glCreateBuffers(1, &this->VBO));
	GLCall(glCreateBuffers(1, &this->EBO));

    GLCall(glNamedBufferData(this->VBO, ( div + 1 ) * ( div + 1 ) * sizeof(struct vertex_t) , NULL, GL_STATIC_DRAW));
	GLCall(glNamedBufferData(this->EBO, div * div * 6 * 4, NULL, GL_STATIC_DRAW));

	this->vertex_buffer = new VertexBuffer(this->VBO);
	this->index_buffer = new IndexBuffer(this->EBO, div * div * 6);
	this->shader.Bind();
	Texture tex(terrainTextureFile);
	tex.Bind();
	this->shader.SetUniform1i("mountain_tex", 0); 
	this->shader.SetUniform1f("fog_density", fog_density); 
	this->shader.SetUniform1i("mountain_tex", 0);
	this->shader.SetUniform3f("sun_direction_vector", sun_dir_m.x , sun_dir_m.y , sun_dir_m.z );

	this->terrain_generator.Bind();
	this->terrain_generator.SetUniform1i("number_of_divs", div);
	this->terrain_generator.SetUniform1f("min_x", min_x);
	this->terrain_generator.SetUniform1f("max_x", max_x);
	this->terrain_generator.SetUniform1f("min_z", min_z);
	this->terrain_generator.SetUniform1f("max_z", max_z);
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

Terrain::~Terrain(){
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->EBO);
}

void Terrain::render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos){
		this->shader.Bind();
		this->tex.Bind();
		this->shader.SetUniformMat4f("ViewProjectionMatrix", ViewProjection   );
		this->shader.SetUniform3f("camera_loc", camera_pos.x, camera_pos.y, camera_pos.z);
		this->vertex_array.Bind();

		GLCall(glDrawElements(GL_TRIANGLES, this->index_buffer->GetCount() , GL_UNSIGNED_INT, nullptr));

		this->tex.Unbind();
		this->vertex_array.Unbind();
		this->vertex_buffer->Unbind();
		this->index_buffer->Unbind();
		this->shader.Unbind();
}	

std::tuple<float,float,float,float> Terrain::get_dimentions() const {
	return std::make_tuple(
		this->min_x_m * mountain_scale_factor_m, 
		this->max_x_m * mountain_scale_factor_m, 
		this->min_z_m * mountain_scale_factor_m,
		this->max_z_m * mountain_scale_factor_m
	);
}

int Terrain::get_divisions() const{
	return div_m;
}

unsigned int Terrain::get_terrain_ssbo_buffer_id() const{
	return this->VBO;
}
