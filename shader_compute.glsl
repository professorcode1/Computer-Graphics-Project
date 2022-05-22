#version 460 core

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;

#define M_PI 3.14159265358979323846264338327950288

struct Vertex{
    vec3 pos;
    float u;
    vec3 nor;
    float v;
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
uniform float lacunarity;
uniform float persistance;
uniform vec3 camera_loc;
uniform vec3 sun_light_dir;
layout ( std430, binding = 0 ) buffer Vertices {Vertex vertices[] ; } vertex_container_object;
layout ( std430, binding = 1 ) buffer Indices {int indices[][6] ;} indices_container_object ;

vec2 fade(vec2 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}

float cnoise(vec2 P){
  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod(Pi, 289.0); // To avoid truncation effects in permutation
  vec4 ix = Pi.xzxz;
  vec4 iy = Pi.yyww;
  vec4 fx = Pf.xzxz;
  vec4 fy = Pf.yyww;
  vec4 i = permute(permute(ix) + iy);
  vec4 gx = 2.0 * fract(i * 0.0243902439) - 1.0; // 1/41 = 0.024...
  vec4 gy = abs(gx) - 0.5;
  vec4 tx = floor(gx + 0.5);
  gx = gx - tx;
  vec2 g00 = vec2(gx.x,gy.x);
  vec2 g10 = vec2(gx.y,gy.y);
  vec2 g01 = vec2(gx.z,gy.z);
  vec2 g11 = vec2(gx.w,gy.w);
  vec4 norm = 1.79284291400159 - 0.85373472095314 * 
    vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11));
  g00 *= norm.x;
  g01 *= norm.y;
  g10 *= norm.z;
  g11 *= norm.w;
  float n00 = dot(g00, vec2(fx.x, fy.x));
  float n10 = dot(g10, vec2(fx.y, fy.y));
  float n01 = dot(g01, vec2(fx.z, fy.z));
  float n11 = dot(g11, vec2(fx.w, fy.w));
  vec2 fade_xy = fade(Pf.xy);
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}


void fractal_sum(inout vec3 pos, inout vec3 nor){
    //pos.y = pos.x + pos.z;
    //return;
    double epsilon = 1 / (number_of_divs * input_shrink_fctr * 100);
    float x = pos.x;
    float z = pos.z;
    bool made_normal = false;
    for(int iter_i = 0 ; iter_i < 32 ; iter_i++){
        if( (ActiveWaveFreqsGround & (1 << iter_i)) != 0){
            float freq = pow(lacunarity, iter_i);
            float amp = pow(persistance, iter_i);
            float u = freq * ( cos( rotation_Angle * iter_i ) * x - sin( rotation_Angle * iter_i ) * z );
            float v = freq * ( sin( rotation_Angle * iter_i ) * x + cos( rotation_Angle * iter_i ) * z );
            float noise_ = cnoise(vec2(u, v));
            pos.y += noise_ / amp;
            if (! made_normal){
                nor.x += ( (cnoise(vec2(u + freq * cos( rotation_Angle * iter_i ) * epsilon, v + freq * sin( rotation_Angle * iter_i ) * epsilon)) - noise_ ) / amp );
                nor.z += ( (cnoise(vec2(u - freq * sin( rotation_Angle * iter_i ) * epsilon, v + freq * cos( rotation_Angle * iter_i ) * epsilon)) - noise_ ) / amp );
                made_normal = true;
            }
        }
    }
    nor.x /= float(epsilon);
    nor.z /= float(epsilon);
}
int row = int(gl_GlobalInvocationID.x);
int col = int(gl_GlobalInvocationID.y);
int index = row * ( number_of_divs + 1 ) + col;
int indicesIndex = row * number_of_divs + col;
void main(){
    if(row <= number_of_divs && col <= number_of_divs){
        float x = ( min_x + col * ( (max_x - min_x) / number_of_divs) ) / input_shrink_fctr;
        float z = ( min_z + row * ( (max_z - min_z) / number_of_divs) ) / input_shrink_fctr;
        vertex_container_object.vertices[index].pos.x = x ;
        vertex_container_object.vertices[index].pos.y = 0 ;
        vertex_container_object.vertices[index].pos.z = z ;

        vertex_container_object.vertices[index].nor.x = 0 ;
        vertex_container_object.vertices[index].nor.y = 1 ;
        vertex_container_object.vertices[index].nor.z = 0 ;

        fractal_sum(vertex_container_object.vertices[index].pos, vertex_container_object.vertices[index].nor);

        vertex_container_object.vertices[index].pos.y *= output_increase_fctr;

        vertex_container_object.vertices[index].u = fract(x);
        vertex_container_object.vertices[index].v = fract(z);
        
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