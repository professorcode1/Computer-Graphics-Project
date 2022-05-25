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
    vec3 pos;
    float u;
    vec3 nor;
    float v;
};

layout ( std430, binding = 0 ) buffer Vertices {Vertex vertices[] ; } vertex_container_object;
layout ( std430, binding = 1 ) buffer Indices {int indices[][6] ;} indices_container_object ;

float mod289(float x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}
vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

// Permutation polynomial (ring size 289 = 17*17)
float permute(float x) {
  return mod289(((x*34.0)+10.0)*x);
}

// Hashed 2-D gradients with an extra rotation.
// (The constant 0.0243902439 is 1/41)
vec2 rgrad2(vec2 p, float rot) {
#if 0
// Map from a line to a diamond such that a shift maps to a rotation.
  float u = permute(permute(p.x) + p.y) * 0.0243902439 + rot; // Rotate by shift
  u = 4.0 * fract(u) - 2.0;
  // (This vector could be normalized, exactly or approximately.)
  return vec2(abs(u)-1.0, abs(abs(u+1.0)-2.0)-1.0);
#else
// For more isotropic gradients, sin/cos can be used instead.
  float u = permute(permute(p.x) + p.y) * 0.0243902439 + rot; // Rotate by shift
  u = fract(u) * 6.28318530718; // 2*pi
  return vec2(cos(u), sin(u));
#endif
}

vec3 srdnoise(vec2 pos, float rot) {
  // Offset y slightly to hide some rare artifacts
  pos.y += 0.001;
  // Skew to hexagonal grid
  vec2 uv = vec2(pos.x + pos.y*0.5, pos.y);
  
  vec2 i0 = floor(uv);
  vec2 f0 = fract(uv);
  // Traversal order
  vec2 i1 = (f0.x > f0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);

  // Unskewed grid points in (x,y) space
  vec2 p0 = vec2(i0.x - i0.y * 0.5, i0.y);
  vec2 p1 = vec2(p0.x + i1.x - i1.y * 0.5, p0.y + i1.y);
  vec2 p2 = vec2(p0.x + 0.5, p0.y + 1.0);

  // Integer grid point indices in (u,v) space
  i1 = i0 + i1;
  vec2 i2 = i0 + vec2(1.0, 1.0);

  // Vectors in unskewed (x,y) coordinates from
  // each of the simplex corners to the evaluation point
  vec2 d0 = pos - p0;
  vec2 d1 = pos - p1;
  vec2 d2 = pos - p2;

  vec3 x = vec3(p0.x, p1.x, p2.x);
  vec3 y = vec3(p0.y, p1.y, p2.y);
  vec3 iuw = x + 0.5 * y;
  vec3 ivw = y;
  
  // Avoid precision issues in permutation
  iuw = mod289(iuw);
  ivw = mod289(ivw);

  // Create gradients from indices
  vec2 g0 = rgrad2(vec2(iuw.x, ivw.x), rot);
  vec2 g1 = rgrad2(vec2(iuw.y, ivw.y), rot);
  vec2 g2 = rgrad2(vec2(iuw.z, ivw.z), rot);

  // Gradients dot vectors to corresponding corners
  // (The derivatives of this are simply the gradients)
  vec3 w = vec3(dot(g0, d0), dot(g1, d1), dot(g2, d2));
  
  // Radial weights from corners
  // 0.8 is the square of 2/sqrt(5), the distance from
  // a grid point to the nearest simplex boundary
  vec3 t = 0.8 - vec3(dot(d0, d0), dot(d1, d1), dot(d2, d2));

  // Partial derivatives for analytical gradient computation
  vec3 dtdx = -2.0 * vec3(d0.x, d1.x, d2.x);
  vec3 dtdy = -2.0 * vec3(d0.y, d1.y, d2.y);

  // Set influence of each surflet to zero outside radius sqrt(0.8)
  if (t.x < 0.0) {
    dtdx.x = 0.0;
    dtdy.x = 0.0;
	t.x = 0.0;
  }
  if (t.y < 0.0) {
    dtdx.y = 0.0;
    dtdy.y = 0.0;
	t.y = 0.0;
  }
  if (t.z < 0.0) {
    dtdx.z = 0.0;
    dtdy.z = 0.0;
	t.z = 0.0;
  }

  // Fourth power of t (and third power for derivative)
  vec3 t2 = t * t;
  vec3 t4 = t2 * t2;
  vec3 t3 = t2 * t;
  
  // Final noise value is:
  // sum of ((radial weights) times (gradient dot vector from corner))
  float n = dot(t4, w);
  
  // Final analytical derivative (gradient of a sum of scalar products)
  vec2 dt0 = vec2(dtdx.x, dtdy.x) * 4.0 * t3.x;
  vec2 dn0 = t4.x * g0 + dt0 * w.x;
  vec2 dt1 = vec2(dtdx.y, dtdy.y) * 4.0 * t3.y;
  vec2 dn1 = t4.y * g1 + dt1 * w.y;
  vec2 dt2 = vec2(dtdx.z, dtdy.z) * 4.0 * t3.z;
  vec2 dn2 = t4.z * g2 + dt2 * w.z;

  return 11.0*vec3(n, dn0 + dn1 + dn2);
}

vec3 sdnoise(vec2 pos) {
  return srdnoise(pos, 0.0);
}

void fractal_sum(inout vec3 pos, inout vec3 nor){
    //pos.y = pos.x + pos.z;
    //return;
    double epsilon = 1 / (number_of_divs * input_shrink_fctr * 100);
    float x = pos.x;
    float z = pos.z;
    float offset = 0;
    for(int iter_i = 0 ; iter_i < 32 ; iter_i++){
        if( (ActiveWaveFreqsGround & (1 << iter_i)) != 0){
            float freq = pow(lacunarity, iter_i);
            float amp = pow(persistance, iter_i);
            float u = freq * ( cos( rotation_Angle * iter_i ) * x - sin( rotation_Angle * iter_i ) * z );
            float v = freq * ( sin( rotation_Angle * iter_i ) * x + cos( rotation_Angle * iter_i ) * z );
            vec3 data = sdnoise(vec2(u, v));
            float noise_ = data.x;
            pos.y += noise_ / amp;
            //let f = sum i [ {1 / amp ^ i}  * noise(v,u) ] 
            //del f / del x = del f / dev v * del v / dex + delf / del u * del u / delx  
            
            //double del_f_del_u = data.y ;
            //double del_f_del_v =  data.z;
            //double del_u_del_x = freq * cos( rotation_Angle * iter_i );
            //double del_v_del_x = freq * sin( rotation_Angle * iter_i );
            //double del_u_del_z = -1 * freq * sin( rotation_Angle * iter_i );
            //double del_v_del_z =  1 * freq * cos( rotation_Angle * iter_i );
            //double del_f_del_x = del_f_del_u * del_u_del_x + del_f_del_v * del_v_del_x; 
            //double del_f_del_z = del_f_del_u * del_u_del_z + del_f_del_v * del_v_del_z; 
            nor.x -= data.y / amp;
            nor.z -= data.z / amp;
        }
    }
}
int row = int(gl_GlobalInvocationID.x);
int col = int(gl_GlobalInvocationID.y);
void main(){
    int index = VERTICES_IN_PLANE + row * ( number_of_divs + 1 ) + col;
    int indicesIndex = VERTICES_IN_PLANE + row * number_of_divs + col;
    float del_x = ( max_x - min_x ) / ( number_of_divs * input_shrink_fctr );
    float del_z = ( max_z - min_z ) / ( number_of_divs * input_shrink_fctr );
    float x = min_x / input_shrink_fctr + row * del_x ;
    float z = min_z / input_shrink_fctr + col * del_z;
    if(row <= number_of_divs && col <= number_of_divs){
        float del_x = ( max_x - min_x ) / ( number_of_divs * input_shrink_fctr );
        float del_z = ( max_z - min_z ) / ( number_of_divs * input_shrink_fctr );
        float x = min_x / input_shrink_fctr + row * del_x ;
        float z = min_z / input_shrink_fctr + col * del_z;
        vertex_container_object.vertices[index].pos.x = x ;
        vertex_container_object.vertices[index].pos.y = 0 ;
        vertex_container_object.vertices[index].pos.z = z ;

        vertex_container_object.vertices[index].nor.x = 0 ;
        vertex_container_object.vertices[index].nor.y = 1 ;
        vertex_container_object.vertices[index].nor.z = 0 ;

        fractal_sum(vertex_container_object.vertices[index].pos, vertex_container_object.vertices[index].nor);
        vertex_container_object.vertices[index].nor = normalize(vertex_container_object.vertices[index].nor);
        vertex_container_object.vertices[index].pos.y *= output_increase_fctr;

        vertex_container_object.vertices[index].u = fract(x);
        vertex_container_object.vertices[index].v = fract(z);

        

    }
    //memoryBarrier();
    //barrier();
//
    //if(index == 0){
    //    float max_hops = sun_height / sun_light_dir.y;
    //    if(sun_light_dir.x < 0){
    //        
    //    }
    //}

    if(row < number_of_divs && col < number_of_divs){
        
        indices_container_object.indices[indicesIndex][0] = VERTICES_IN_PLANE + index;
        indices_container_object.indices[indicesIndex][1] = VERTICES_IN_PLANE + index + ( number_of_divs + 1 ) + 1 ;
        indices_container_object.indices[indicesIndex][2] = VERTICES_IN_PLANE + index + ( number_of_divs + 1 ) ;
        indices_container_object.indices[indicesIndex][3] = VERTICES_IN_PLANE + index;
        indices_container_object.indices[indicesIndex][4] = VERTICES_IN_PLANE + index + 1 ;
        indices_container_object.indices[indicesIndex][5] = VERTICES_IN_PLANE + index + ( number_of_divs + 1 ) + 1 ;
    }

}