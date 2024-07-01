#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include "vertex_index_layout.h"
#include "../IncGLM.hpp"

void rotate_mesh_inplace(
    std::vector<vertex_t> &mesh,
    const std::vector<float> &angles, 
    const std::vector<std::string> &axis 
);