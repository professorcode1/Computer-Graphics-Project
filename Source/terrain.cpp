#include "terrain.h"
#include "waveFrontFileApi.h"

static float epsilon = 0.001;

static float hash( float n )
{
    return glm::fract(sin(n)*43758.5453);
}


static float noise(const glm::vec3 x){
	using namespace glm;
	vec3 p = floor(x);
    vec3 f = fract(x);

    f = f*f*( vec3(3.0f,3.0f,3.0f) - (f * 2.0f));
    float n = p.x + p.y*57.0 + 113.0*p.z;

    return mix(mix(mix( hash(n+0.0), hash(n+1.0),f.x),
                   mix( hash(n+57.0), hash(n+58.0),f.x),f.y),
               mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                   mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);

}

static float generate_terrain_inner(
	float x,float z,
	const std::vector<int>  &ActiveWaveNumber,
	float lacunarity,float persistance, float rotation_Angle
){
    float res = 0;
    for(const int iter_i: ActiveWaveNumber){
            float freq = pow(lacunarity, iter_i);
            float amp = pow(persistance, iter_i);
            float u = freq * ( cos( rotation_Angle * iter_i ) * x - sin( rotation_Angle * iter_i ) * z );
            float v = freq * ( sin( rotation_Angle * iter_i ) * x + cos( rotation_Angle * iter_i ) * z );
            float noise_ = noise( glm::vec3( u , 0 , v ) );
            res += noise_ / amp;
    }
    return res;
}


static float generate_terrain_inner(
	const glm::vec3 &vector,
	const std::vector<int>  &ActiveWaveNumber,
	float lacunarity,float persistance, float rotation_Angle

){
    return generate_terrain_inner(vector.x, vector.z, ActiveWaveNumber,  lacunarity,persistance, rotation_Angle);
}

static void genereate_terrain_vertex(
	vertex_t* vertex,
	int div, int length_of_side,
	int row,int col,
	glm::vec2 &terrain_index,
	const float input_shrink_fctr,
	const float  output_increase_fctr,
	glm::mat4 &MountainModelMatrix,
	const std::vector<int>  &ActiveWaveNumber,
	float lacunarity,float persistance, float rotation_Angle

){
    float del_x = ( length_of_side ) / (div + 0 );
    float del_z = ( length_of_side ) / (div + 0 );
    float min_x = length_of_side * (terrain_index.x - 0.5);
    float min_z = length_of_side * (terrain_index.y - 0.5);
    float x = min_x  + row * del_x ;
    float z = min_z  + col * del_z;
    float epsilon_del_x = epsilon * del_x;
    float epsilon_del_z = epsilon * del_z;
	vertex->pos[0] = x;
	vertex->pos[1] = 0;
	vertex->pos[2] = z;
	const glm::vec3 input_(
        vertex->pos[0] / input_shrink_fctr,
        0,
        vertex->pos[2] / input_shrink_fctr
    );
    float noise_output = generate_terrain_inner( 
		input_.x, input_.z, 
		ActiveWaveNumber, lacunarity,persistance, rotation_Angle
	);
    vertex->pos[1] = noise_output * output_increase_fctr ;
    glm::vec4 pos;
    pos.x = vertex->pos[0];
    pos.y = vertex->pos[1];
    pos.z = vertex->pos[2];
    pos.w = 1.0;
    pos = MountainModelMatrix * pos;
    vertex->pos[0] = pos.x;
    vertex->pos[1] = pos.y;
    vertex->pos[2] = pos.z;

	glm::vec3 normal;
    normal.x = ( 
		generate_terrain_inner( 
			input_ + glm::vec3( 0 , 0 , epsilon_del_z ), 
			ActiveWaveNumber, lacunarity,
			persistance, rotation_Angle
		) - noise_output 
	) / epsilon_del_z ;
    normal.y = 1 ;
    normal.z = ( 
		generate_terrain_inner( 
			input_ + glm::vec3( epsilon_del_x , 0 , 0 ) , 
			ActiveWaveNumber, lacunarity,
			persistance, rotation_Angle
		) - noise_output 
	) / epsilon_del_x ;
    normal *= output_increase_fctr;
    normal = glm::normalize(normal);

	vertex->nor[0] = normal.x;
	vertex->nor[1] = normal.y;
	vertex->nor[2] = normal.z;

	vertex->uv[0] = col%2;
	vertex->uv[1] = row%2;

	// std::cout<<vertex->pos[0]<<std::endl;
	// std::cout<<vertex->pos[1]<<std::endl;
	// std::cout<<vertex->pos[2]<<std::endl;
}

static void generate_the_terrain(
	const std::vector<int>  &ActiveWaveNumber, float rotation_angle_fractal_ground,
	float output_increase_fctr_, float input_shrink_fctr_, 
    float lacunarity, float persistance,
	int div, 
	float length_of_side, 
	float Mountain_Scale_Factor,
	glm::vec2 &terrain_index,
	vertex_t* vertex_buffer_cpu,
    index_t* index_buffer_cpu
){

	glm::mat4 mountain_model;
	mountain_model = glm::scale(glm::mat4(1.0f), glm::vec3(Mountain_Scale_Factor, Mountain_Scale_Factor, Mountain_Scale_Factor));
	for(int row_index=0 ; row_index<=div;row_index++){
		for(int col_index=0;col_index<=div;col_index++){
			const int index = row_index * ( div + 1 ) + col_index;
		    int indicesIndex = row_index * div + col_index;
			genereate_terrain_vertex(
				vertex_buffer_cpu + index,
				div,length_of_side, row_index, col_index, terrain_index,
				input_shrink_fctr_, output_increase_fctr_,
				mountain_model, ActiveWaveNumber,
				lacunarity, persistance, rotation_angle_fractal_ground
			);

			if(row_index == div || col_index == div){
				continue;
			}

			index_buffer_cpu[indicesIndex].indices[0] = index;
        	index_buffer_cpu[indicesIndex].indices[1] = index + ( div + 1 ) + 1 ;
        	index_buffer_cpu[indicesIndex].indices[2] = index + ( div + 1 ) ;
        	index_buffer_cpu[indicesIndex].indices[3] = index;
        	index_buffer_cpu[indicesIndex].indices[4] = index + 1 ;
        	index_buffer_cpu[indicesIndex].indices[5] = index + ( div + 1 ) + 1;
		}
	}
}

TerrainPatch::TerrainPatch( 
	float fog_density , 
    const std::vector<int>  &ActiveWaveNumber, float rotation_angle_fractal_ground,
	float output_increase_fctr_, float input_shrink_fctr_, 
    float lacunarity, float persistance,
	bool writeToFile,int div, 
	float length_of_side, 
	float Mountain_Scale_Factor, 
    const glm::vec3 &sun_direction,
	glm::vec2 terrain_index,
    const bool async_generation,
    const std::string &vertexShaderFile, 
	const std::string &fragmentShaderFile
):
shader(vertexShaderFile, fragmentShaderFile),
div_m{div},
length_of_side_m{length_of_side},
mountain_scale_factor_m{Mountain_Scale_Factor},
sun_dir_m{sun_direction},
terrain_index_m{terrain_index}
{
    VertexBufferLayout vertex_layout_simple;
	CREATE_VERTEX_LAYOUT(vertex_layout_simple);

    


	this->shader.Bind();
	// Texture tex(terrainTextureFile);
	this->shader.SetUniform1i("mountain_tex", 0); 
	this->shader.SetUniform1f("fog_density", fog_density); 
	this->shader.SetUniform1i("mountain_tex", 0);
	this->shader.SetUniform3f("sun_direction_vector", sun_dir_m.x , sun_dir_m.y , sun_dir_m.z );

    glm::mat4 mountain_model;
	mountain_model = glm::scale(glm::mat4(1.0f), glm::vec3(Mountain_Scale_Factor, Mountain_Scale_Factor, Mountain_Scale_Factor));
	this->vertex_buffer_cpu = new vertex_t[(div_m+1)*(div_m+1)];
	this->index_buffer_cpu = new index_t[div_m * div_m];
	generate_the_terrain(
		ActiveWaveNumber,
		rotation_angle_fractal_ground,output_increase_fctr_,
		input_shrink_fctr_, lacunarity,
		persistance,div, length_of_side,Mountain_Scale_Factor, 
		terrain_index, vertex_buffer_cpu, index_buffer_cpu
	);
	this->vertex_buffer = new VertexBuffer(
		static_cast<void*>(vertex_buffer_cpu), 
		(div_m+1) * (div_m+1) * sizeof(vertex_t)
	);
	this->index_buffer = new IndexBuffer(
		static_cast<unsigned int*>(
			static_cast<void*>(
				index_buffer_cpu
			)
		), 
		div_m * div_m * 6
	);
	this->vertex_array.AddBuffer(*vertex_buffer, vertex_layout_simple);
	this->vertex_array.AddElementBuffer(*index_buffer);



	if(writeToFile)
		write_to_file(this->VBO,this->EBO,div);
	// std::cout<<min_x<<" "<< max_x<<" "<<  min_z<<" "<<  max_z<<std::endl;

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
	delete [] this->vertex_buffer_cpu;
	delete [] this->index_buffer_cpu;

}

void TerrainPatch::render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos){
		this->shader.Bind();
		this->shader.SetUniformMat4f("ViewProjectionMatrix", ViewProjection   );
		this->shader.SetUniform3f("camera_loc", camera_pos.x, camera_pos.y, camera_pos.z);
		this->vertex_array.Bind();

		// std::cout<<"terrain patch render called"<<std::endl;
		GLCall(glDrawElements(GL_TRIANGLES, this->index_buffer->GetCount() , GL_UNSIGNED_INT, nullptr));

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
	generate_the_terrain(
		ActiveWaveNumber,
		rotation_angle_fractal_ground,output_increase_fctr_,
		input_shrink_fctr_, lacunarity,
		persistance,div_m, length_of_side,Mountain_Scale_Factor, 
		terrain_index_m, vertex_buffer_cpu, index_buffer_cpu
	);


	this->vertex_buffer->rewrite_data(vertex_buffer_cpu, (div_m+1) * (div_m+1)* sizeof(vertex_t));
	this->index_buffer->rewrite_data(index_buffer_cpu, div_m * div_m * 6);
}