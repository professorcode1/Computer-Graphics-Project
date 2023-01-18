#include "meshTransform.h"

static glm::vec3 axis_vector(char axis){
    switch (axis)
    {
    case 'x':
        return glm::vec3(1.0, 0.0, 0.0);
    case 'y':
        return glm::vec3(0.0, 1.0, 0.0);
    case 'z':
        return glm::vec3(0.0, 0.0, 1.0);    
    default:
        assert(false);
        return glm::vec3(0.0, 0.0, 0.0); //so compiler doesn't complain
    }
}

static glm::mat4 generate_rotation_matrix(
    const std::array<float, 3> &angles, 
    const std::array<char, 3> &axis
){
    glm::mat4 model = glm::mat4(1.0f);
    for(int i=2 ; i>=0 ; i--){
        model = glm::rotate(model, angles[i], axis_vector(axis[i]));
    }
    return model;
}

template <class T>
void rotate_mesh_inplace(
    std::vector<T> &mesh,
    const std::array<float, 3> &angles, 
    const std::array<char, 3> &axis 
){
    const glm::mat4 rotation_matrix = generate_rotation_matrix(angles, axis);
    for(auto &triangle_data : mesh){
        glm::vec3* triangle = std::static_pointer_cast<glm::vec3*>(triangle_data.pos);
        *triangle = rotation_matrix * (*triangle); 
    }
}


template 
void rotate_mesh_inplace(
    std::vector<vertex_t> &mesh,
    const std::array<float, 3> &angles, 
    const std::array<char, 3> &axis 
);