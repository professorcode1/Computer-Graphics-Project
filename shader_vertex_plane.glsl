#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in float texture_u;
layout(location = 2) in vec3 normal;
layout(location = 3) in float texture_v;

uniform mat4 MVP_plane;

out vec2 v_TexCoord;

void main(){
	gl_Position = MVP_plane * position;
	v_TexCoord = vec2(texture_u, texture_v);
}