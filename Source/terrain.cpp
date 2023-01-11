#include "terrain.h"
#include "waveFrontFileApi.h"

Terrain::Terrain( const std::string &terrainGeneratorShaderFile, const std::string &vertexShaderFile, const std::string &fragmentShaderFile ,const VertexBufferLayout &vertex_layout,
    const std::string &terrainTextureFile,float fog_density , 
    const std::vector<int>  &ActiveWaveNumber, float rotation_angle_fractal_ground,float output_increase_fctr_, float input_shrink_fctr_, 
    float lacunarity, float persistance,bool writeToFile,int div, float min_x,float max_x,float min_z,float max_z, float Mountain_Scale_Factor):
	terrain_generator(terrainGeneratorShaderFile), 
	shader(vertexShaderFile, fragmentShaderFile),
	tex(terrainTextureFile)
	{
    
    
	glCreateVertexArrays(1, &this->VAO);
	glCreateBuffers(1, &this->VBO);
	glCreateBuffers(1, &this->EBO);

    glNamedBufferData(this->VBO, ( div + 1 ) * ( div + 1 ) * sizeof(struct vertex_t) , NULL, GL_STATIC_DRAW);
	glNamedBufferData(this->EBO, div * div * 6 * 4, NULL, GL_STATIC_DRAW);

	this->vertex_buffer = new VertexBuffer(this->VBO);
	this->index_buffer = new IndexBuffer(this->EBO, div * div * 6);
	this->shader.Bind();
	Texture tex(terrainTextureFile);
	tex.Bind();
	this->shader.SetUniform1i("mountain_tex", 0); 
	this->shader.SetUniform1f("fog_density", fog_density); 
	this->shader.SetUniform1i("mountain_tex", 0);
	
	this->vertex_array.AddBuffer(*vertex_buffer, vertex_layout);

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
	this->terrain_generator.bindSSOBuffer(0, vertex_buffer->GetRenderedID());
	this->terrain_generator.bindSSOBuffer(1, index_buffer->GetRenderedID());
	this->terrain_generator.launch_and_Sync(ceil((float)(div +1)/8), ceil((float)(div +1)/4), 1);
	this->mountain_model = glm::scale(glm::mat4(1.0f), glm::vec3(Mountain_Scale_Factor, Mountain_Scale_Factor, Mountain_Scale_Factor));
	this->shader.Bind();
	this->shader.SetUniformMat4f("ModelMatrix", this->mountain_model);
		
	if(writeToFile)
		write_to_file(this->VBO,this->EBO,div);
	// std::cout<<min_x<<" "<< max_x<<" "<<  min_z<<" "<<  max_z<<std::endl;
}

void Terrain::render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos){
		this->shader.Bind();
		this->tex.Bind();
		this->shader.SetUniformMat4f("ModelViewProjectionMatrix", ViewProjection * this->mountain_model  );
		this->shader.SetUniform3f("camera_loc", camera_pos.x, camera_pos.y, camera_pos.z);
		this->vertex_array.Bind();
		this->index_buffer->Bind();

		GLCall(glDrawElements(GL_TRIANGLES, this->index_buffer->GetCount() , GL_UNSIGNED_INT, nullptr));

}