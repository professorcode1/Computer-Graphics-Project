#version 460 core

layout(location = 0) out vec4 color;
in float intensity;
vec4 ambience =  vec4(0.1,0.1,0.1,0);
vec4 dirt_color = vec4(155.0 / 256.0, 118.0 / 256.0, 83.0 / 256.0 , 0);
void main(){
	color = dirt_color * intensity + 1 * ambience ;
	
}