#version 460 core

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;

#define M_PI 3.14159265358979323846264338327950288


uniform int number_of_divs;
uniform float min_x;
uniform float max_x;
uniform float min_z;
uniform float max_z;
uniform int ActiveWaveFreqsGround;
uniform float rotation_Angle;
uniform float output_increase_fctr;
uniform float input_shrink_fctr;
uniform float lacunarity;
uniform float persistance;
uniform vec3 camera_loc;
uniform vec3 sun_light_dir;
uniform float sun_height;
shared float hops_along_x;
shared float hops_along_z;
uniform int VERTICES_IN_PLANE;
struct Vertex{
    float posx,posy,posz;
    float norx, nory,norz;
    float u,v;
};

layout ( std430, binding = 0 ) buffer Vertices {Vertex vertices[] ; } vertex_container_object;
layout ( std430, binding = 1 ) buffer Indices {int indices[][6] ;} indices_container_object ;
float hash( float n )
{
    return fract(sin(n)*43758.5453);
}

float noise( vec3 x )
{
    // The noise function returns a value in the range -1.0f -> 1.0f

    vec3 p = floor(x);
    vec3 f = fract(x);

    f       = f*f*(3.0-2.0*f);
    float n = p.x + p.y*57.0 + 113.0*p.z;

    return mix(mix(mix( hash(n+0.0), hash(n+1.0),f.x),
                   mix( hash(n+57.0), hash(n+58.0),f.x),f.y),
               mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                   mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
}
int row = int(gl_GlobalInvocationID.x);
int col = int(gl_GlobalInvocationID.y);
float epsilon = 0.001;
void main(){
    int index = VERTICES_IN_PLANE + row * ( number_of_divs + 1 ) + col;
    int indicesIndex = VERTICES_IN_PLANE + row * number_of_divs + col;
    if(row <= number_of_divs && col <= number_of_divs){
        float del_x = ( max_x - min_x ) / ( number_of_divs  );
        float del_z = ( max_z - min_z ) / ( number_of_divs  );
        float x = min_x  + row * del_x ;
        float z = min_z  + col * del_z;
        float epsilon_del_x = epsilon * del_x;
        float epsilon_del_z = epsilon * del_z;

        vertex_container_object.vertices[index].posx = x ;
        vertex_container_object.vertices[index].posy = 0 ;
        vertex_container_object.vertices[index].posz = z ;

        vec3 input_ = vec3(vertex_container_object.vertices[index].posx, vertex_container_object.vertices[index].posy, vertex_container_object.vertices[index].posz) / input_shrink_fctr;
        float noise_output = noise(input_);
        vertex_container_object.vertices[index].posy = noise_output * output_increase_fctr ;
        
        vec3 normal;

        normal.x = ( noise( input_ + vec3( 0 , 0 , epsilon_del_z ) ) - noise_output ) / epsilon_del_z ;
        normal.y = 1 ;
        normal.z = ( noise( input_ + vec3( epsilon_del_x , 0 , 0 ) ) - noise_output ) / epsilon_del_x ;

        normalize(normal);

        vertex_container_object.vertices[index].norx = normal.x;
        vertex_container_object.vertices[index].nory = normal.y;
        vertex_container_object.vertices[index].norz = normal.z;

        vertex_container_object.vertices[index].u = fract( ( float( row ) * 100.5 ) / number_of_divs );
        vertex_container_object.vertices[index].v = fract( ( float( col ) * 100.5 ) / number_of_divs );

    }

    if(row < number_of_divs && col < number_of_divs){
        
        indices_container_object.indices[indicesIndex][0] = VERTICES_IN_PLANE + index;
        indices_container_object.indices[indicesIndex][1] = VERTICES_IN_PLANE + index + ( number_of_divs + 1 ) + 1 ;
        indices_container_object.indices[indicesIndex][2] = VERTICES_IN_PLANE + index + ( number_of_divs + 1 ) ;
        indices_container_object.indices[indicesIndex][3] = VERTICES_IN_PLANE + index;
        indices_container_object.indices[indicesIndex][4] = VERTICES_IN_PLANE + index + 1 ;
        indices_container_object.indices[indicesIndex][5] = VERTICES_IN_PLANE + index + ( number_of_divs + 1 ) + 1 ;
    }

}