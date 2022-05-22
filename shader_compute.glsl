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
uniform float lacunarity;
uniform float persistance;

layout ( std430, binding = 0 ) buffer Vertices {Vertex vertices[] ; } vertex_container_object;
layout ( std430, binding = 1 ) buffer Indices {int indices[][6] ;} indices_container_object ;

vec2 fade(vec2 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}

vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}

vec2 fade_diff(vec2 t){return 30 * t * t *( t * ( t - 2 ) + 1 );}

vec4 permute_diff(vec4 x){return 68*x + 1;}

float cnoise_and_grad(in vec2 P,inout vec2 Normal){
    vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
    vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
    Pi = mod(Pi, 289.0); // To avoid truncation effects in permutation

    vec4 del_Pi_del_x = vec4(0, 0, 0, 0);//the differential of floor function is 0
    vec4 del_Pi_del_y = vec4(0, 0, 0, 0);//the differential of floor function is 0
    vec4 del_Pf_del_x = vec4(1, 0, 1, 0);
    vec4 del_Pf_del_y = vec4(0, 1, 0, 1);

    vec4 ix = Pi.xzxz;
    vec4 iy = Pi.yyww;
    vec4 fx = Pf.xzxz;
    vec4 fy = Pf.yyww;
    

    vec4 del_ix_del_x = del_Pi_del_x.xzxz;
    vec4 del_iy_del_x = del_Pi_del_x.yyww;
    vec4 del_fx_del_x = del_Pf_del_x.xzxz;
    vec4 del_fy_del_x = del_Pf_del_x.yyww;
    
    vec4 del_ix_del_y = del_Pi_del_y.xzxz;
    vec4 del_iy_del_y = del_Pi_del_y.yyww;
    vec4 del_fx_del_y = del_Pf_del_y.xzxz;
    vec4 del_fy_del_y = del_Pf_del_y.yyww;

    vec4 i = permute(permute(ix) + iy);

    vec4 del_i_del_x = permute_diff(permute(ix) + iy) * (permute_diff(ix) * del_ix_del_x + del_iy_del_x);
    vec4 del_i_del_y = permute_diff(permute(ix) + iy) * (permute_diff(ix) * del_ix_del_y + del_iy_del_y);

    vec4 gx_prev = 2.0 * fract(i * 0.0243902439) - 1.0; // 1/41 = 0.024...
    vec4 gy = abs(gx_prev) - 0.5;
    vec4 tx = floor(gx_prev + 0.5);
    vec4 gx = gx_prev - tx;

    vec4 del_gx_prev_del_x = 2.0 * del_i_del_x * 0.0243902439;
    vec4 del_gx_prev_del_y = 2.0 * del_i_del_y * 0.0243902439;
    vec4 del_gy_del_x, del_gy_del_y;
    
    if (gx_prev.x > 0){
        del_gy_del_x.x =  1 * del_gx_prev_del_x.x;
        del_gy_del_y.x =  1 * del_gx_prev_del_y.x;
    }else{
        del_gy_del_x.x = -1 * del_gx_prev_del_x.x;
        del_gy_del_y.x = -1 * del_gx_prev_del_y.x;
    }
    if (gx_prev.y > 0){
        del_gy_del_x.y =  1 * del_gx_prev_del_x.y;
        del_gy_del_y.y =  1 * del_gx_prev_del_y.y;
    }else{
        del_gy_del_x.y = -1 * del_gx_prev_del_x.y;
        del_gy_del_y.y = -1 * del_gx_prev_del_y.y;
    }
    if (gx_prev.z > 0){
        del_gy_del_x.z =  1 * del_gx_prev_del_x.z;
        del_gy_del_y.z =  1 * del_gx_prev_del_y.z;
    }else{
        del_gy_del_x.z = -1 * del_gx_prev_del_x.z;
        del_gy_del_y.z = -1 * del_gx_prev_del_y.z;
    }
    if (gx_prev.w > 0){
        del_gy_del_x.w =  1 * del_gx_prev_del_x.w;
        del_gy_del_y.w =  1 * del_gx_prev_del_y.w;
    }else{
        del_gy_del_x.w = -1 * del_gx_prev_del_x.w;
        del_gy_del_y.w = -1 * del_gx_prev_del_y.w;
    }
    vec4 del_tx_del_x = vec4(0, 0, 0, 0); //the differential of floor function is 0
    vec4 del_tx_del_y = vec4(0, 0, 0, 0); //the differential of floor function is 0
    
    vec4 del_gx_del_x = del_gx_prev_del_x - del_tx_del_x;
    vec4 del_gx_del_y = del_gx_prev_del_y - del_tx_del_y;

    vec2 g00_prev = vec2(gx.x,gy.x);
    vec2 g10_prev = vec2(gx.y,gy.y);
    vec2 g01_prev = vec2(gx.z,gy.z);
    vec2 g11_prev = vec2(gx.w,gy.w);

    vec2 del_g00_prev_del_x = vec2(del_gx_del_x.x, del_gy_del_x.x);
    vec2 del_g01_prev_del_x = vec2(del_gx_del_x.y, del_gy_del_x.y);
    vec2 del_g10_prev_del_x = vec2(del_gx_del_x.z, del_gy_del_x.z);
    vec2 del_g11_prev_del_x = vec2(del_gx_del_x.w, del_gy_del_x.w);

    vec2 del_g00_prev_del_y = vec2(del_gx_del_y.x, del_gy_del_y.x);
    vec2 del_g01_prev_del_y = vec2(del_gx_del_y.y, del_gy_del_y.y);
    vec2 del_g10_prev_del_y = vec2(del_gx_del_y.z, del_gy_del_y.z);
    vec2 del_g11_prev_del_y = vec2(del_gx_del_y.w, del_gy_del_y.w);

    vec4 norm = 1.79284291400159 - 0.85373472095314 * vec4(dot(g00_prev, g00_prev), dot(g01_prev, g01_prev), dot(g10_prev, g10_prev), dot(g11_prev, g11_prev));


    vec4 del_norm_del_x = -2.0 * 0.85373472095314 * vec4(dot(g00_prev, del_g00_prev_del_x), 
                                                         dot(g01_prev, del_g01_prev_del_x), 
                                                         dot(g10_prev, del_g10_prev_del_x), 
                                                         dot(g11_prev, del_g11_prev_del_x));
    vec4 del_norm_del_y = -2.0 * 0.85373472095314 * vec4(dot(g00_prev, del_g00_prev_del_y), 
                                                         dot(g01_prev, del_g01_prev_del_y), 
                                                         dot(g10_prev, del_g10_prev_del_y), 
                                                         dot(g11_prev, del_g11_prev_del_y));

    vec2 g00 = g00_prev * norm.x;
    vec2 g01 = g10_prev * norm.y;
    vec2 g10 = g01_prev * norm.z;
    vec2 g11 = g11_prev * norm.w;

    vec2 del_g00_del_x = del_g00_prev_del_x * norm.x + g00_prev * del_norm_del_x.x;
    vec2 del_g01_del_x = del_g01_prev_del_x * norm.y + g01_prev * del_norm_del_x.y;
    vec2 del_g10_del_x = del_g10_prev_del_x * norm.z + g10_prev * del_norm_del_x.z;
    vec2 del_g11_del_x = del_g11_prev_del_x * norm.w + g11_prev * del_norm_del_x.w;

    vec2 del_g00_del_y = del_g00_prev_del_y * norm.x + g00_prev * del_norm_del_y.x;
    vec2 del_g01_del_y = del_g01_prev_del_y * norm.y + g01_prev * del_norm_del_y.y;
    vec2 del_g10_del_y = del_g10_prev_del_y * norm.z + g10_prev * del_norm_del_y.z;
    vec2 del_g11_del_y = del_g11_prev_del_y * norm.w + g11_prev * del_norm_del_y.w;

    float n00 = dot(g00, vec2(fx.x, fy.x));
    float n10 = dot(g10, vec2(fx.y, fy.y));
    float n01 = dot(g01, vec2(fx.z, fy.z));
    float n11 = dot(g11, vec2(fx.w, fy.w));
    
    float del_n00_del_x = dot(del_g00_del_x, vec2(fx.x, fy.x)) + dot(g00, vec2(del_fx_del_x.x, del_fy_del_x.x));
    float del_n10_del_x = dot(del_g10_del_x, vec2(fx.y, fy.y)) + dot(g10, vec2(del_fx_del_x.y, del_fy_del_x.y));
    float del_n01_del_x = dot(del_g01_del_x, vec2(fx.z, fy.z)) + dot(g01, vec2(del_fx_del_x.z, del_fy_del_x.z));
    float del_n11_del_x = dot(del_g11_del_x, vec2(fx.w, fy.w)) + dot(g11, vec2(del_fx_del_x.w, del_fy_del_x.w));

    float del_n00_del_y = dot(del_g00_del_y, vec2(fx.x, fy.x)) + dot(g00, vec2(del_fx_del_y.x, del_fy_del_y.x));
    float del_n10_del_y = dot(del_g10_del_y, vec2(fx.y, fy.y)) + dot(g10, vec2(del_fx_del_y.y, del_fy_del_y.y));
    float del_n01_del_y = dot(del_g01_del_y, vec2(fx.z, fy.z)) + dot(g01, vec2(del_fx_del_y.z, del_fy_del_y.z));
    float del_n11_del_y = dot(del_g11_del_y, vec2(fx.w, fy.w)) + dot(g11, vec2(del_fx_del_y.w, del_fy_del_y.w));

    vec2 fade_xy = fade(Pf.xy);

    vec2 del_fade_xy_del_x = fade_diff(vec2(Pf.x, 0));
    vec2 del_fade_xy_del_y = fade_diff(vec2(0, Pf.y));
    
    
    vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
    vec2 del_n_x_del_x = mix(vec2(del_n00_del_x, del_n01_del_x), vec2(del_n10_del_x, del_n11_del_x), fade_xy.x) + del_fade_xy_del_x.x * (vec2(n10, n11) - vec2(n00, n01));
    vec2 del_n_x_del_y = mix(vec2(del_n00_del_y, del_n01_del_y), vec2(del_n10_del_y, del_n11_del_y), fade_xy.x) + del_fade_xy_del_y.x * (vec2(n10, n11) - vec2(n00, n01));
    
    float n_xy = mix(n_x.x, n_x.y, fade_xy.y);

    float del_n_xy_del_x = mix(del_n_x_del_x.x, del_n_x_del_x.y, fade_xy.y) + del_fade_xy_del_x.y * ( n_x.y - n_x.x );
    float del_n_xy_del_y = mix(del_n_x_del_y.x, del_n_x_del_y.y, fade_xy.y) + del_fade_xy_del_y.y * ( n_x.y - n_x.x );
    
    Normal.x = 2.3 * del_n_xy_del_x;
    Normal.y = 2.3 * del_n_xy_del_y;
    return 2.3 * n_xy;
}

void fractal_sum(inout vec4 pos, inout vec4 nor){
    //pos.y = pos.x + pos.z;
    //return;
    double epsilon = 1 / (number_of_divs * input_shrink_fctr * 100);
    float x = pos.x;
    float z = pos.z;
    float offset = 0;
    for(int iter_i = 0 ; iter_i < 32 ; iter_i++){
        if( (ActiveWaveFreqsGround & (1 << iter_i)) != 0){
            vec2 Normal;
            float freq = pow(lacunarity, iter_i);
            float amp = pow(persistance, iter_i);
            float u = freq * ( cos( rotation_Angle * iter_i ) * x - sin( rotation_Angle * iter_i ) * z );
            float v = freq * ( sin( rotation_Angle * iter_i ) * x + cos( rotation_Angle * iter_i ) * z );
            float noise_ = cnoise_and_grad(vec2(u, v), Normal);
            pos.y += noise_ / amp;
            //let f = sum i [ {1 / amp ^ i}  * noise(v,u) ] 
            //del f / del x = del f / dev v * del v / dex + delf / del u * del u / delx  
            
            double del_f_del_u = Normal.x;
            double del_f_del_v = Normal.y;
            double del_u_del_x = freq * cos( rotation_Angle * iter_i );
            double del_v_del_x = freq * sin( rotation_Angle * iter_i );
            double del_u_del_z = -1 * freq * sin( rotation_Angle * iter_i );
            double del_v_del_z =  1 * freq * cos( rotation_Angle * iter_i );
            double del_f_del_x = del_f_del_u * del_u_del_x + del_f_del_v * del_v_del_x; 
            double del_f_del_z = del_f_del_u * del_u_del_z + del_f_del_v * del_v_del_z; 
            nor.x -= float(del_f_del_x);
            nor.z -= float(del_f_del_z);
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
        vertex_container_object.vertices[index].nor.y = 1;
        vertex_container_object.vertices[index].nor.z = 0;
        vertex_container_object.vertices[index].nor.w = 1;

        fractal_sum(vertex_container_object.vertices[index].pos, vertex_container_object.vertices[index].nor);

        vertex_container_object.vertices[index].pos.y *= output_increase_fctr;
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
