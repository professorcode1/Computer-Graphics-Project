#version 460 core

layout(location = 0) out vec4 color;

in float intensity;

void main(){
	color = vec4( intensity , intensity , intensity , 1.0 );
}