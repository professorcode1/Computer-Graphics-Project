#include <glm/mat4x4.hpp>
#include <bits/stdc++.h>
#include <glm/gtx/rotate_vector.hpp>
#include "vertex_index_layout.h"

void rotate_mesh_inplace(
    std::vector<vertex_t> &mesh,
    const std::vector<float> &angles, 
    const std::vector<std::string> &axis 
);