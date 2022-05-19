#version 460 core

layout(location = 0) out vec4 color;
flat in float intensity;
vec4 ambience =  vec4(0.1,0.1,0.1,0);
void main(){
	//color = vec4(0, (intensity + 1 )/2,0, 0.0);
	//color = vec4(0, 0,0, 0.0);
	color = vec4((intensity + 1 )/2, (intensity + 1 )/2, (intensity + 1 )/2, 0.0) + ambience;
	//color = vec4(1,1,1, 0.0);
}