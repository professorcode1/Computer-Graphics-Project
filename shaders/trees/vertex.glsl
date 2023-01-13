#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture_uv;

uniform mat4 VPMatrix;
uniform mat4 ModelMatrix;
uniform vec3 camera_loc;
uniform vec3 sun_direction_vector;


out vec2 v_TexCoord;
out float intensity;
out float camera_dist;

void main(){
	gl_Position = VPMatrix * ModelMatrix * position;
	intensity = dot(normalize(sun_direction_vector), normal) ;
	intensity = max(0.4, intensity); //ambient light
	v_TexCoord = texture_uv;
	camera_dist = length((ModelMatrix * position).xyz - camera_loc);
}