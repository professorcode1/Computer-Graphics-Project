#include "trees.h"
#include "cloud.h"
#include "terrain.h"
#include "../OpenGL/Texture.h"

class Grid{
private:
    static constexpr int  NumberOfPatcherInGridPerAxis = 5  ;
    Trees* tree_grid[Grid::NumberOfPatcherInGridPerAxis][Grid::NumberOfPatcherInGridPerAxis];
    TerrainPatch* main_terrain_grid[Grid::NumberOfPatcherInGridPerAxis][Grid::NumberOfPatcherInGridPerAxis];
    // Clouds* cloud_grid[Grid::NumberOfPatcherInGridPerAxis][Grid::NumberOfPatcherInGridPerAxis];
    Texture* tex;
    glm::vec2 current_center;

public:
    const glm::vec2 get_current_center(){ return this->current_center;};
    void generate_terrain_grid(
        const glm::vec2 &center, const float fog_densty, 
        const glm::vec3 &sun_direction, const nlohmann::json &terrainParam
    );
    void generate_trees(
	    const float fog_densty, const glm::vec3 &sun_direction, 
	    const int trees_per_division,const int tress_scale
    );
    Grid(
        const float fog_densty, const glm::vec3 &sun_direction, 
        const nlohmann::json &terrainParam, 
        const nlohmann::json &tressParameter, 
        const nlohmann::json &cloudParameters, 
        const glm::vec2 current_center= glm::vec2(0.0,0.0),
        const std::string &textureFile = "assets/Ground Textures/soil.jpg"
    );
    void render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos);
    void update(	uint32_t time, const glm::vec3 &plane_position,
	const float fog_densty, const glm::vec3 &sun_direction, 
	const nlohmann::json &terrainParam);
};