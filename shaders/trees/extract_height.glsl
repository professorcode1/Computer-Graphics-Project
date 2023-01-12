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
    int terrain_index_left_low = down_z_div + left_x_div * ( number_of_divs + 1 );
    float y = terrain_vertex_container_object.vertices[terrain_index_left_low].posy;
    tree_position_container_object.positions[ index ].y = y;

}