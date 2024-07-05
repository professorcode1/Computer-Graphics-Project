#include "Grid.h"

static TerrainPatch* terrain_patch_generator(
	const float fog_densty, const glm::vec3 &sun_direction, 
	const nlohmann::json &terrainParam, const glm::vec2 &center ,
	const bool async_generation
){
	const int number_of_divisions = terrainParam.at("divisions").get<int>();
	std::cout<<"generating terrain patch for "<<number_of_divisions<<" divisions"<<std::endl;
	return new TerrainPatch(
		fog_densty,terrainParam["wave numbers active"], 
		terrainParam.at("rotation angle fractal ground"), terrainParam.at("output_increase_fctr_"), terrainParam.at("input_shrink_fctr_"), 
		terrainParam.at("lacunarity"), terrainParam.at("persistance"),number_of_divisions, 
		terrainParam.at("length of one side"), 
		terrainParam.at("Mountain Scale Factor"), sun_direction, center, async_generation
		);
}

void Grid::generate_terrain_grid(
    const glm::vec2 &center, const float fog_densty, 
    const glm::vec3 &sun_direction, const nlohmann::json &terrainParam
){
	const float offset_to_terrain_path =  Grid::NumberOfPatcherInGridPerAxis / 2;
	for(int i=0 ; i < Grid::NumberOfPatcherInGridPerAxis  ; i++){
		for(int j=0 ; j < Grid::NumberOfPatcherInGridPerAxis ; j++){
			
			this->main_terrain_grid[ i ][ j ] = terrain_patch_generator(
				fog_densty, 
				sun_direction, 
				terrainParam, 
				glm::vec2(
					center.x + i - offset_to_terrain_path, 
					center.y + j - offset_to_terrain_path
				),
				true
			);	

		}
	}
}

void Grid::generate_trees(
	const float fog_densty, const glm::vec3 &sun_direction, 
	const int trees_per_division,const int tress_scale
){
	for(int i=0 ; i < Grid::NumberOfPatcherInGridPerAxis ; i++){
		for(int j=0 ; j < Grid::NumberOfPatcherInGridPerAxis ; j++){
			
			this->tree_grid[ i ][ j ] = new Trees(
				trees_per_division,
				tress_scale,
				*this->main_terrain_grid[ i ][ j ], 
				sun_direction,fog_densty
			);
		}
	}
}
Grid::Grid(
	const float fog_densty, const glm::vec3 &sun_direction, 
	const nlohmann::json &terrainParam, const nlohmann::json &tressParameter,
    const nlohmann::json &cloudParameters,
    const glm::vec2 current_center,
	const std::string &textureFile
):current_center(current_center){
	this->tex = new Texture(textureFile);
	this->generate_terrain_grid(current_center, fog_densty,sun_direction,terrainParam);
	this->generate_trees(
		fog_densty,sun_direction, 
		tressParameter.at("tress per division").get<int>(), 
		tressParameter.at("tress scale").get<float>()
	);
}

void Grid::render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos){
	this->tex->Bind();
	for(int i=0;i<Grid::NumberOfPatcherInGridPerAxis;i++){
		for(int j=0;j<Grid::NumberOfPatcherInGridPerAxis;j++){
			this->main_terrain_grid[i][j]->render(ViewProjection, camera_pos);
            this->tree_grid[ i ][ j ]->render(ViewProjection, camera_pos);
            // this->cloud_grid[ i ][ j ]->render(ViewProjection);
		}
	}
	this->tex->Unbind();
}

void Grid::update(
	uint32_t time, const glm::vec3 &plane_position,
	const float fog_densty, const glm::vec3 &sun_direction, 
	const nlohmann::json &terrainParam
){
	const int offset_to_terrain_path =  Grid::NumberOfPatcherInGridPerAxis / 2;
	float min_x, min_z, max_x, max_z;
	std::tie(min_x, max_x, min_z, max_z) = this->main_terrain_grid[ offset_to_terrain_path ][ offset_to_terrain_path ]->get_corners();
	const float plane_x = plane_position.x, plane_z = plane_position.z;
	if(min_x <= plane_x && plane_x <= max_x && min_z <= plane_z && plane_z <= max_z){
		return ;
	}

	const glm::vec2 movement_vector = glm::vec2(
		-1.0 * static_cast<int>(min_x > plane_x) + static_cast<float>(max_x  < plane_x),
		-1.0 * static_cast<int>(min_z > plane_z) + static_cast<float>(max_z  < plane_z)
	);
	const glm::vec2 new_center = this->current_center + movement_vector;

	this->current_center = new_center;

	for(int i=0 ; i < Grid::NumberOfPatcherInGridPerAxis ; i++){
		for(int j=0 ; j < Grid::NumberOfPatcherInGridPerAxis ; j++){

			this->main_terrain_grid[ i ][ j ]->set_index(
				glm::vec2(
					new_center.x + i - offset_to_terrain_path, 
					new_center.y + j - offset_to_terrain_path
				),
				terrainParam["wave numbers active"], 
				terrainParam.at("rotation angle fractal ground"),
				terrainParam.at("output_increase_fctr_"), terrainParam.at("input_shrink_fctr_"), 
				terrainParam.at("lacunarity"), terrainParam.at("persistance"), 
				terrainParam.at("length of one side"), 
				terrainParam.at("Mountain Scale Factor")
			);
			this->tree_grid[ i ][ j ]->position_the_trees(
				this->main_terrain_grid[ i ][ j ]->get_divisions(),
				this->main_terrain_grid[ i ][ j ]->get_vertex_buffer_cpu()
			);
		}
	}

}