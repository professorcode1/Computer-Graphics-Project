#include "meshTransform.h"

static glm::vec3 axis_vector(std::string axis){
    switch (axis[0])
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
    const std::vector<float> &angles, 
    const std::vector<std::string> &axis
){
    glm::mat4 model = glm::mat4(1.0f);
    for(int i=0 ; i<3 ; i++){
        model = glm::rotate(model, glm::radians(angles[i]), axis_vector(axis[i]));
    }
    return model;
}

void rotate_mesh_inplace(
    std::vector<vertex_t> &mesh,
    const std::vector<float> &angles, 
    const std::vector<std::string> &axis 
){
    const glm::mat4 rotation_matrix = generate_rotation_matrix(angles, axis);
    for(auto &triangle_data : mesh){
        glm::vec3* triangle = (glm::vec3*)(triangle_data.pos);
        *triangle = glm::vec3(rotation_matrix * glm::vec4(*triangle, 1.0)); 
    }
}
