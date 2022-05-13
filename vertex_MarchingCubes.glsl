#version 330 core

#define M_PI 3.1415926535897932384626433832795


uniform int N;
uniform float rotation_Angle;

float smoothStep(float a, float b, float x){
    float lambda = min(1, max(0 , (x - a) / ( b -a )  )  );
    return lambda * lambda * ( 3 - 2 * lambda );
}

float a_i_j(float i, float j){
    float u = 50 * i / M_PI;
    float v = 50 * j / M_PI;
    float ans_hlpr;
    modf(u * v * ( u + v ), ans_hlpr); 
    return 2 * ans_hlpr - 1;
}

float noise_Function( vec2 position ){
    float x_frac, z_frac;
    float i = modf(position.x, x_frac) , j = modf(position.y, z_frac);
    float a = a_i_j(i    , j     );
    float b = a_i_j(i + 1, j     );
    float c = a_i_j(i    , j + 1 );
    float d = a_i_j(i + 1, j + 1 );
    float S_x = smoothStep(0, 1, x_frac), S_z = smoothStep(0, 1, z_frac);
    return a + ( b - a ) * S_x + ( c - a ) * S_z + (a - b - c + d) * S_x * S_z;
}

float terrain_Function( vec2 xz ){
    float ans = 0;
    for(int iter_i = 0 ; iter_i < N ; iter_i ++){
        mat2 rotation_matrix;
        rotation_matrix[ 0 ][ 0 ] =  cos(rotation_Angle * iter_i);
        rotation_matrix[ 0 ][ 1 ] = -sin(rotation_Angle * iter_i);
        rotation_matrix[ 1 ][ 0 ] =  sin(rotation_Angle * iter_i);
        rotation_matrix[ 1 ][ 1 ] =  cos(rotation_Angle * iter_i);
        
        ans += noise_Function(rotation_matrix * xz * pow(2, iter_i)) / pow(2, iter_i);
    }
    return ans;
}

layout(location = 0) in vec2 position;
uniform mat4 transformation;

void main(){
    vec4 hlpr;
    hlpr.xz = position.xy;
    hlpr.y = 0;
    hlpr.w = 1;
    gl_Position = transformation * hlpr; 
    gl_PointSize = 50;
}