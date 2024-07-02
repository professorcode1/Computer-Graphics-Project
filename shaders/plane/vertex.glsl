#version 300 es	
precision mediump float;


layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture_uv;

uniform mat4 MVP_plane;

out vec2 v_TexCoord;

void main(){
	gl_Position = MVP_plane * position;
	v_TexCoord = texture_uv;
}