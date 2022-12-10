#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in float texture_u;
layout(location = 2) in vec3 normal;
layout(location = 3) in float texture_v;

const vec3  kSunDir = vec3(-0.624695,0.468521,-0.624695);

uniform mat4 MVP_mountain;
uniform vec3 camera_loc;



out vec2 v_TexCoord;
out float intensity;
out float camera_dist;

void main(){
	gl_Position = MVP_mountain * position;
	intensity = dot(normalize(kSunDir), normal) ;
	v_TexCoord = vec2(texture_u, texture_v);
	camera_dist = length(position.xyz - camera_loc);
}