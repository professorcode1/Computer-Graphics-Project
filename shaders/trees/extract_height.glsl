#version 460 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

uniform int number_of_divs;
uniform int number_of_trees_sqrt;


struct Vertex{
    float posx,posy,posz;
    float norx, nory,norz;
    float u,v;
};
struct Position{
    float x,y,z;
    float nx,ny,nz;
};

layout ( std430, binding = 0 ) buffer Vertices {Vertex vertices[] ; } terrain_vertex_container_object;
layout ( std430, binding = 1 ) buffer Positions {Position positions[] ;} tree_position_container_object ;

int row = int(gl_GlobalInvocationID.x);
int col = int(gl_GlobalInvocationID.y);
void main(){
    if(row > number_of_trees_sqrt || col > number_of_trees_sqrt)
        return ;
    int index = col + row * number_of_trees_sqrt;
    int div_row_to_sample = int(tree_position_container_object.positions[index].x) ;
    int div_col_to_sample = int(tree_position_container_object.positions[index].z) ;
    int terrain_index = div_col_to_sample + div_row_to_sample * ( number_of_divs + 1 );

    tree_position_container_object.positions[index].x = 
        terrain_vertex_container_object.vertices[terrain_index].posx;

    tree_position_container_object.positions[index].y = 
        terrain_vertex_container_object.vertices[terrain_index].posy;

    tree_position_container_object.positions[index].z = 
        terrain_vertex_container_object.vertices[terrain_index].posz;

    tree_position_container_object.positions[index].nx = 
        terrain_vertex_container_object.vertices[terrain_index].norx;

    tree_position_container_object.positions[index].ny = 
        terrain_vertex_container_object.vertices[terrain_index].nory;

    tree_position_container_object.positions[index].nz = 
        terrain_vertex_container_object.vertices[terrain_index].norz;

}