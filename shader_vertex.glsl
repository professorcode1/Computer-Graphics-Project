#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
const vec3  kSunDir = vec3(-0.624695,0.468521,-0.624695);

uniform mat4 MVP;

out float intensity;

void main(){
	gl_Position = MVP * position;
	vec3 normal_ = vec3(normal.xyz);
	intensity = (0, dot(normal_, kSunDir) / (length(normal) * length(kSunDir)));
	//intensity = 1;
}