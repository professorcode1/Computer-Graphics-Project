#version 460 core

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;

#define M_PI 3.14159265358979323846264338327950288

struct Vertex{
    vec4 pos;
    vec4 nor;
};


uniform int number_of_divs;
uniform float min_x;
uniform float max_x;
uniform float min_z;
uniform float max_z;
uniform int ActiveWaveFreqsGround;
uniform float rotation_Angle;
uniform float output_increase_fctr;
uniform float input_shrink_fctr;

layout ( std430, binding = 0 ) buffer Vertices {Vertex vertices[] ; } vertex_container_object;
layout ( std430, binding = 1 ) buffer Indices {int indices[][6] ;} indices_container_object ;

float smoothStep( float a, float b, float x ){
    float lambda = min( 1, max( 0, ( x - a ) / ( b - a ) ) );
    //return lambda * lambda * lambda * ( lambda * ( lambda * 6.0 - 15.0 ) + 10.0 );
    return lambda * lambda * ( 3.0 - 2.0 * lambda );
}

float a_i_j(in float i, in float j){
    float u = 50 * i / M_PI ;
    float v = 50 * j / M_PI ;
    float needless_var;
    float hlpr = modf( u * v * ( u + v ), needless_var);
    return 2 * hlpr - 1;
}

void Noise(inout vec4 pos, inout vec4 nor, in float x, in float z ,in int iter_i){
    float i, j;
    //float frac_x = modf(x, i), frac_z = modf(z, j);
    float frac_x = smoothStep(0, 1, modf(x, i)), frac_z = smoothStep(0, 1, modf(z, j));
    
    float a = a_i_j(i    , j    );
    float b = a_i_j(i + 1, j + 0);
    float c = a_i_j(i + 0, j + 1);
    float d = a_i_j(i + 1, j + 1);
    float y_incr = a + ( b - a ) * frac_x + ( c - a ) * frac_z + ( a - b - c + d ) * frac_z * frac_x;
    y_incr /= pow(2, iter_i);
    y_incr *= output_increase_fctr;
    pos.y += y_incr;
    
    float del_f_del_x = cos( rotation_Angle * iter_i ) * ( ( b - a ) + z * ( a - b - c + d ) ) + 
                            sin( rotation_Angle * iter_i ) * ( ( c - a ) + x * ( a - b - c + d ) ) ;
    float del_f_del_z = -1 * sin( rotation_Angle * iter_i ) * ( ( b - a ) + z * ( a - b - c + d ) ) + cos( rotation_Angle * iter_i ) * ( ( c - a ) + x * ( a - b - c + d ) ) ;
    nor.x -= del_f_del_x;
    nor.z -= del_f_del_z;

}

void fractal_sum(inout vec4 pos, inout vec4 nor){
    //pos.y = pos.x + pos.z;
    //return;
    float x = pos.x;
    float z = pos.z;
    for(int iter_i = 0 ; iter_i < 32 ; iter_i++){
        if( (ActiveWaveFreqsGround & (1 << iter_i)) != 0){
            float hlpr_x = pow(2, iter_i) * ( cos( rotation_Angle * iter_i ) * x - sin( rotation_Angle * iter_i ) * z );
            float hlpr_z = pow(2, iter_i) * ( sin( rotation_Angle * iter_i ) * x + cos( rotation_Angle * iter_i ) * z );
            //pos.y += hlpr_x + hlpr_z;
            Noise(pos, nor, hlpr_x, hlpr_z, iter_i);
        }
    }
}
int row = int(gl_GlobalInvocationID.x);
int col = int(gl_GlobalInvocationID.y);
int index = row * ( number_of_divs + 1 ) + col;
int indicesIndex = row * number_of_divs + col;
void main(){
    if(row <= number_of_divs && col <= number_of_divs){
        vertex_container_object.vertices[index].pos.x = ( min_x + col * ( (max_x - min_x) / number_of_divs) ) / input_shrink_fctr;
        vertex_container_object.vertices[index].pos.y = 0;
        vertex_container_object.vertices[index].pos.z = ( min_z + row * ( (max_z - min_z) / number_of_divs) ) / input_shrink_fctr;
        vertex_container_object.vertices[index].pos.w = 1;

        vertex_container_object.vertices[index].nor.x = 0;
        vertex_container_object.vertices[index].nor.y = 0;
        vertex_container_object.vertices[index].nor.z = 0;
        vertex_container_object.vertices[index].nor.w = 1;

        fractal_sum(vertex_container_object.vertices[index].pos, vertex_container_object.vertices[index].nor);
    }
    
    if(row < number_of_divs && col < number_of_divs){
        
        indices_container_object.indices[indicesIndex][0] = index;
        indices_container_object.indices[indicesIndex][1] = index + ( number_of_divs + 1 ) + 1 ;
        indices_container_object.indices[indicesIndex][2] = index + ( number_of_divs + 1 ) ;

        indices_container_object.indices[indicesIndex][3] = index;
        indices_container_object.indices[indicesIndex][4] = index + 1 ;
        indices_container_object.indices[indicesIndex][5] = index + ( number_of_divs + 1 ) + 1 ;
    }

}
