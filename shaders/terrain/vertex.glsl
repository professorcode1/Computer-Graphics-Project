#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture_uv;

const vec3  kSunDir = vec3(-0.624695,0.468521,-0.624695);

uniform mat4 MVP_mountain;
uniform vec3 camera_loc;



out vec2 v_TexCoord;
out float intensity;
out float camera_dist;

void main(){
	gl_Position = MVP_mountain * position;
	intensity = dot(normalize(kSunDir), normal) ;
	intensity = max(0.4, intensity); //ambient light

	v_TexCoord = texture_uv;
	camera_dist = length(position.xyz - camera_loc);
}