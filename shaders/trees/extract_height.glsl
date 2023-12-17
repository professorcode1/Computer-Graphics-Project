#version 460 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

uniform int number_of_divs;
uniform int number_of_trees_sqrt;
uniform uint tree_per_divison_per_axis;
uint hash(uint seed){
    seed = (seed ^ 61u) ^ (seed >> 16u);
    seed *= 9u;
    seed = seed ^ (seed >> 4u);
    seed *= 0x27d4eb2du;
    seed = seed ^ (seed >> 15u);
    return seed;
};

uint random_int(uint seed, uint range){
    return hash(seed) % range;
};

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
    if(row > number_of_trees_sqrt || col > number_of_trees_sqrt)
        return ;
    int index = col + row * number_of_trees_sqrt;
    uint div_row_to_sample = row * tree_per_divison_per_axis + random_int(
        floatBitsToUint(
            terrain_vertex_container_object.vertices[0].posx
        ) + hash(row) + hash(col) , tree_per_divison_per_axis);
    uint div_col_to_sample = col * tree_per_divison_per_axis + random_int(
        floatBitsToUint(
            terrain_vertex_container_object.vertices[0].posz
        )+ hash(row) + hash(col), tree_per_divison_per_axis);
    uint terrain_index = div_col_to_sample + div_row_to_sample * ( number_of_divs + 1 );
    
    tree_position_container_object.positions[index].x = terrain_vertex_container_object.vertices[terrain_index].posx;
    tree_position_container_object.positions[index].y = terrain_vertex_container_object.vertices[terrain_index].posy;
    tree_position_container_object.positions[index].z = terrain_vertex_container_object.vertices[terrain_index].posz;

}