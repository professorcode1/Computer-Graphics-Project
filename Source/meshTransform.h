#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>
#include <bits/stdc++.h>
#include <glm/trigonometric.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include "vertex_index_layout.h"

template <class T>
void rotate_mesh_inplace(
    std::vector<T> &mesh,
    const std::array<float, 3> &angles, 
    const std::array<char, 3> &axis 
);