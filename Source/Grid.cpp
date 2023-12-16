#include "Grid.h"

static TerrainPatch* terrain_patch_generator(
	const float fog_densty, const glm::vec3 &sun_direction, 
	const nlohmann::json &terrainParam, const glm::vec2 &center ,
	const bool async_generation
){
	return new TerrainPatch(
		terrainParam.at("noise texture file"), fog_densty,terrainParam["wave numbers active"], 
		terrainParam.at("rotation angle fractal ground"), terrainParam.at("output_increase_fctr_"), terrainParam.at("input_shrink_fctr_"), 
		terrainParam.at("lacunarity"), terrainParam.at("persistance"),terrainParam.at("write to file"), terrainParam.at("divisions"), 
		terrainParam.at("length of one side"), 
		terrainParam.at("Mountain Scale Factor"), sun_direction, center, async_generation
		);
}

Grid::Grid(
	const float fog_densty, const glm::vec3 &sun_direction, 
	const nlohmann::json &terrainParam, const nlohmann::json &tressParameter,
    const nlohmann::json &cloudParameters
):current_center(0.0, 0.0){
	const float offset_to_terrain_path =  Grid::NumberOfPatcherInGridPerAxis / 2;
	for(int i=0 ; i < Grid::NumberOfPatcherInGridPerAxis ; i++){
		for(int j=0 ; j < Grid::NumberOfPatcherInGridPerAxis ; j++){
			this->main_terrain_grid[ i ][ j ] = terrain_patch_generator(
				fog_densty, 
				sun_direction, 
				terrainParam, 
				glm::vec2(
					i - offset_to_terrain_path, 
					j - offset_to_terrain_path
				),
				true
			);	

		}
	}
	for(int i=0 ; i < Grid::NumberOfPatcherInGridPerAxis ; i++){
		for(int j=0 ; j < Grid::NumberOfPatcherInGridPerAxis ; j++){
			
			this->main_terrain_grid[ i ][ j ]->sync();
			this->tree_grid[ i ][ j ] = new Trees(
				tressParameter.at("tress per division").get<int>(),
				tressParameter.at("tress scale").get<float>(),
				tressParameter.at("align with normals").get<bool>(),
				*this->main_terrain_grid[ i ][ j ], 
				sun_direction,fog_densty
			);
        	const float terrain_max_height = 
                terrainParam.at("output_increase_fctr_").get<float>() * 
                terrainParam.at("Mountain Scale Factor").get<float>();

            this->cloud_grid[ i ][ j ] = new Clouds(
                cloudParameters.at("cloud per division").get<uint32_t>(),
                cloudParameters.at("scale").get<float>(),
                this->main_terrain_grid[ i ][ j ]->get_corners(),
                sun_direction,
                terrain_max_height + cloudParameters.at("distance above terrain").get<float>(),
		        cloudParameters.at("input shrink factor"),
		        cloudParameters.at("time shrink factor"),
		        cloudParameters.at("velocity"),
		        cloudParameters.at("rotational velocity degree")
            );

		}
	}
	// for(int i=0 ; i < Grid::NumberOfPatcherInGridPerAxis + 2 ; i++){
	// 	for(int j=0 ; j < Grid::NumberOfPatcherInGridPerAxis + 2 ; j++){
	// 		if( i == 0 || j == 0 || i == Grid::NumberOfPatcherInGridPerAxis +1 || j == Grid::NumberOfPatcherInGridPerAxis +1 ){
	// 			this->outter_terrain_grid[ i ] [ j ] = terrain_patch_generator(
	// 				fog_densty, 
	// 				sun_direction, 
	// 				terrainParam, 
	// 				glm::vec2(
	// 					i - offset_to_terrain_path - 1, 
	// 					j - offset_to_terrain_path - 1
	// 				),
	// 				true
	// 			);
	// 		}else{
	// 			this->outter_terrain_grid[ i ] [ j ] = nullptr;
	// 		}
	// 	}
	// }
}

void Grid::render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos){
	for(int i=0;i<Grid::NumberOfPatcherInGridPerAxis;i++){
		for(int j=0;j<Grid::NumberOfPatcherInGridPerAxis;j++){
			this->main_terrain_grid[i][j]->render(ViewProjection, camera_pos);
            this->tree_grid[ i ][ j ]->render(ViewProjection, camera_pos);
            this->cloud_grid[ i ][ j ]->render(ViewProjection);
		}
	}
}

void Grid::update(uint32_t time){
	for(int i=0;i<Grid::NumberOfPatcherInGridPerAxis;i++){
		for(int j=0;j<Grid::NumberOfPatcherInGridPerAxis;j++){
            this->cloud_grid[ i ][ j ]->flow(time);
        }
    }
}