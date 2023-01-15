#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

uniform mat4 MVPMatrix;
uniform vec3 sun_direction_vector;


out float intensity;

void main(){
	gl_Position = MVPMatrix * position;
	intensity = dot(normalize(sun_direction_vector), normal) ;
	intensity = max(0.3, intensity); //ambient light
}