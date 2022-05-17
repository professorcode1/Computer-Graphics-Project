#version 460 core

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;

#define M_PI 3.14159265358979323846264338327950288

struct Vertex{
    vec4 pos;
    vec4 nor;
};

struct Parameters{
    int number_of_divs;
    float min_x;
    float max_x;
    float min_z;
    float max_z;
    int ActiveWaveFreqsGround;
    float rotation_Angle;
    int number_of_threads_engaged;
};

layout ( std430, binding = 0 ) buffer Vertices {Vertex vertices[] ; };
layout ( std430, binding = 1 ) buffer Indices {int indices[][6] ;} ;
layout ( std430, binding = 2 ) buffer Params { Parameters parameters;};

float smoothStep( float a, float b, float x ){
    float lambda = min( 1, max( 0, ( x - a ) / ( a - b ) ) );
    return lambda * lambda * ( 3 - 2 * lambda );
}

float a_i_j(in float i, in float j){
    float u = 50 * i / M_PI ;
    float v = 50 * j / M_PI ;
    float needless_var;
    float hlpr = modf( u * v * ( u + v ), needless_var);
    return 2 * hlpr - 1;
}

void Noise(inout vec4 pos, inout vec4 nor, in float x, in float z ,in int iter_i){
    float i,j;
    float frac_x = modf(x, i), frac_z = modf(z, j);
    float a = a_i_j(i    , j    );
    float b = a_i_j(i + 1, j    );
    float c = a_i_j(i    , j + 1);
    float d = a_i_j(i + 1, j + 1);
    
    float y_incr = a + ( b - a ) * smoothStep(0, 1, frac_x) + ( c - a ) * smoothStep(0, 1, frac_z) + ( a - b - c + d ) * smoothStep(0, 1, frac_z) * smoothStep(0, 1, frac_x);
    y_incr /= pow(2, iter_i);
    pos.y += y_incr;
    
    float del_f_del_x = cos( parameters.rotation_Angle * iter_i ) * ( ( b - a ) + z * ( a - b - c + d ) ) + 
                            sin( parameters.rotation_Angle * iter_i ) * ( ( c - a ) + x * ( a - b - c + d ) ) ;
    float del_f_del_z = -1 * sin( parameters.rotation_Angle * iter_i ) * ( ( b - a ) + z * ( a - b - c + d ) ) + cos( parameters.rotation_Angle * iter_i ) * ( ( c - a ) + x * ( a - b - c + d ) ) ;
    nor.x -= del_f_del_x;
    nor.z -= del_f_del_z;

}

void fractal_sum(inout vec4 pos, inout vec4 nor){
    float x = pos.x;
    float z = pos.z;
    for(int iter_i = 0 ; iter_i < 32 ; iter_i++){
        if( (parameters.ActiveWaveFreqsGround & (1 << iter_i)) != 0){
            float hlpr_x = pow(2, iter_i) * ( cos( parameters.rotation_Angle * iter_i ) * x - sin( parameters.rotation_Angle * iter_i ) * z );
            float hlpr_z = pow(2, iter_i) * ( sin( parameters.rotation_Angle * iter_i ) * x + cos( parameters.rotation_Angle * iter_i ) * z );
            Noise(pos, nor, hlpr_x, hlpr_z, iter_i);
        }
    }
}


void main(){
    int row = int(gl_GlobalInvocationID.x);
    int col = int(gl_GlobalInvocationID.y);
    int index = row * ( parameters.number_of_divs + 1 ) + col ;
    int indicesIndex = row * parameters.number_of_divs + col;

    atomicAdd(parameters.number_of_threads_engaged, 1);
    if(row <= parameters.number_of_divs && col <= parameters.number_of_divs){
        float delta_x = (parameters.max_x - parameters.min_x) / parameters.number_of_divs ;
        float delta_z = (parameters.max_z - parameters.min_z) / parameters.number_of_divs ;
        float x = parameters.min_x + delta_x * row ;
        float z = parameters.min_z + delta_z * col ;

        vertices[index].pos.x = x;
        vertices[index].pos.y = 0;
        vertices[index].pos.z = z;
        vertices[index].pos.w = 1;

        vertices[index].nor.x = 0;
        vertices[index].nor.y = 0;
        vertices[index].nor.z = 0;
        vertices[index].nor.w = 1;

        fractal_sum(vertices[index].pos, vertices[index].nor);
    }
    if(row < parameters.number_of_divs && col < parameters.number_of_divs){
        indices[indicesIndex][0] = index;
        indices[indicesIndex][1] = index + ( parameters.number_of_divs + 1 ) + 1;
        indices[indicesIndex][2] = index + ( parameters.number_of_divs + 1 );

        indices[indicesIndex][3] = index;
        indices[indicesIndex][4] = index + 1;
        indices[indicesIndex][5] = index + ( parameters.number_of_divs + 1 ) + 1;

    }
    
}