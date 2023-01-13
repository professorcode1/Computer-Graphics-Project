#version 460 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

uniform int number_of_divs;
uniform int number_of_trees_sqrt;
uniform float min_x;
uniform float max_x;
uniform float min_z;
uniform float max_z;
struct Vertex{
    float posx,posy,posz;
    float norx, nory,norz;
    float u,v;
};
struct Position{
    float x,y,z;
};

layout ( std430, binding = 0 ) buffer Vertices {Vertex vertices[] ; } terrain_vertex_container_object;
layout ( std430, binding = 1 ) buffer Positions {Position positions[] ;} tree_position_container_object ;

int row = int(gl_GlobalInvocationID.x);
int col = int(gl_GlobalInvocationID.y);
void main(){
    if(row >= number_of_trees_sqrt || col >= number_of_trees_sqrt)
        return ;
    int index = col + row * number_of_trees_sqrt;
    float x = tree_position_container_object.positions[ index ].x;
    float z = tree_position_container_object.positions[ index ].z;
    int left_x_div = int(floor(((x - min_x) / (max_x - min_x)) * number_of_divs));
    int down_z_div = int(floor(((z - min_z) / (max_z - min_z)) * number_of_divs));
    int terrain_index_l_d = down_z_div + left_x_div * ( number_of_divs + 1 );
    int terrain_index_l_u = down_z_div + 1 + left_x_div * ( number_of_divs + 1 );
    int terrain_index_r_d = down_z_div + (left_x_div + 1) * ( number_of_divs + 1 );
    int terrain_index_r_u = down_z_div + 1 + (left_x_div + 1) * ( number_of_divs + 1 );

    float y_l_d = terrain_vertex_container_object.vertices[terrain_index_l_d].posy;
    float y_r_d = terrain_vertex_container_object.vertices[terrain_index_l_u].posy;
    float y_l_u = terrain_vertex_container_object.vertices[terrain_index_r_d].posy;
    float y_r_u = terrain_vertex_container_object.vertices[terrain_index_r_u].posy;

    float dist_l_d = length(
        vec2(
            terrain_vertex_container_object.vertices[terrain_index_l_d].posx,
            terrain_vertex_container_object.vertices[terrain_index_l_d].posz
        ) - 
        vec2(x,z)
    );
    
    float dist_r_d = length(
        vec2(
            terrain_vertex_container_object.vertices[terrain_index_l_u].posx,
            terrain_vertex_container_object.vertices[terrain_index_l_u].posz
        ) - 
        vec2(x,z)
    );
    float dist_l_u = length(
        vec2(
            terrain_vertex_container_object.vertices[terrain_index_r_d].posx,
            terrain_vertex_container_object.vertices[terrain_index_r_d].posz
        ) - 
        vec2(x,z)
    );
    float dist_r_u = length(
        vec2(
            terrain_vertex_container_object.vertices[terrain_index_r_u].posx,
            terrain_vertex_container_object.vertices[terrain_index_r_u].posz
        ) - 
        vec2(x,z)
    );


    float y = (y_l_d * dist_l_d) + (y_r_d * dist_r_d) + (y_l_u * dist_l_u) + (y_r_u * dist_r_u) / 
                (dist_l_d + dist_r_d + dist_l_u + dist_r_u);

    tree_position_container_object.positions[ index ].y = min(min(y_l_d,y_r_d),min(y_l_u, y_r_u));
    // tree_position_container_object.positions[ index ].y = y;
    tree_position_container_object.positions[ index ].y = y_l_d  + y_r_d  + y_l_u  + y_r_u;
}