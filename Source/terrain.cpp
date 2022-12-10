#include "terrain.h"
#include "waveFrontFileApi.h"

Terrain::Terrain( const std::string &terrainGeneratorShaderFile, const std::string &vertexShaderFile, const std::string &fragmentShaderFile ,const VertexBufferLayout &vertex_layout,
    const std::string &terrainTextureFile,float atmosphere_light_damping_constant, float atmosphere_damping_red_weight,float atmosphere_damping_green_weight,float atmosphere_damping_blue_weight , 
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

	ComputeShader terrain_generator("shader_compute.glsl");
	this->vertex_buffer = new VertexBuffer(this->VBO);
	this->index_buffer = new IndexBuffer(this->EBO, div * div * 6);
	Shader shader("shader_vertex.glsl", "shader_fragment.glsl");
	shader.Bind();
	Texture tex(terrainTextureFile);
	tex.Bind();
	shader.SetUniform1i("mountain_tex", 0); 
	shader.SetUniform1f("atmosphere_light_damping_constant", atmosphere_light_damping_constant); 
	shader.SetUniform4f("atmosphere_light_damping_RGB_Weight", atmosphere_damping_red_weight, atmosphere_damping_green_weight, atmosphere_damping_blue_weight, 1.0); 
	shader.SetUniform1i("mountain_tex", 0);
	
	this->vertex_array.AddBuffer(*vertex_buffer, vertex_layout);

	terrain_generator.Bind();
	terrain_generator.SetUniform1i("number_of_divs", div);
	terrain_generator.SetUniform1f("min_x", min_x);
	terrain_generator.SetUniform1f("max_x", max_x);
	terrain_generator.SetUniform1f("min_z", min_z);
	terrain_generator.SetUniform1f("max_z", max_z);
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
	terrain_generator.bindSSOBuffer(0, vertex_buffer->GetRenderedID());
	terrain_generator.bindSSOBuffer(1, index_buffer->GetRenderedID());
	terrain_generator.launch_and_Sync(ceil((float)(div +1)/8), ceil((float)(div +1)/4), 1);
	this->mountain_model = glm::scale(glm::mat4(1.0f), glm::vec3(Mountain_Scale_Factor, Mountain_Scale_Factor, Mountain_Scale_Factor));
		
	if(writeToFile)
		write_to_file(this->VBO,this->EBO,div);
	// std::cout<<min_x<<" "<< max_x<<" "<<  min_z<<" "<<  max_z<<std::endl;
}

void Terrain::render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos){
		shader.Bind();
		tex.Bind();
		shader.SetUniformMat4f("MVP_mountain", ViewProjection * mountain_model);
		shader.SetUniform3f("camera_loc", camera_pos.x, camera_pos.y, camera_pos.z);
		vertex_array.Bind();
		index_buffer->Bind();

		GLCall(glDrawElements(GL_TRIANGLES, index_buffer->GetCount() , GL_UNSIGNED_INT, nullptr));

}