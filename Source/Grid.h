#include "trees.h"
#include "cloud.h"
#include "terrain.h"

class Grid{
private:
    static constexpr int  NumberOfPatcherInGridPerAxis = 3;
    Trees* tree_grid[Grid::NumberOfPatcherInGridPerAxis][Grid::NumberOfPatcherInGridPerAxis];
    TerrainPatch* main_terrain_grid[Grid::NumberOfPatcherInGridPerAxis][Grid::NumberOfPatcherInGridPerAxis];
    Clouds* cloud_grid[Grid::NumberOfPatcherInGridPerAxis][Grid::NumberOfPatcherInGridPerAxis];
    
    TerrainPatch* outter_terrain_grid[Grid::NumberOfPatcherInGridPerAxis+2][Grid::NumberOfPatcherInGridPerAxis+2];
    glm::vec2 current_center;
public:
    Grid(const float fog_densty, const glm::vec3 &sun_direction, const nlohmann::json &terrainParam, const nlohmann::json &tressParameter, const nlohmann::json &cloudParameters);
    void render(const glm::mat4 &ViewProjection, const glm::vec3 &camera_pos);
    void update(uint32_t time);
};