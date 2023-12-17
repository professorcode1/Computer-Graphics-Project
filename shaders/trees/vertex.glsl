#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture_uv;

uniform mat4 VPMatrix;
uniform samplerBuffer LocationArrayTexture;
uniform vec3 camera_loc;
uniform vec3 sun_direction_vector;
uniform float scaling_factor;
uniform int LocationArrayTextureIndex;


out vec2 v_TexCoord;
out float intensity;
out float camera_dist;

mat4 scale_and_translate(vec3 translation, float scale) {
    return mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(translation, 1.0)
    ) * mat4(
        vec4(scale, 0.0, 0.0, 0.0),
        vec4(0.0, scale, 0.0, 0.0),
        vec4(0.0, 0.0, scale, 0.0),
        vec4(1.0, 1.0, 1.0, 1.0)
    );
}

void main(){
	vec4 tranlsation_data = texelFetch(LocationArrayTexture, LocationArrayTextureIndex);

	mat4 ModelMatrix = scale_and_translate(tranlsation_data.xyz, scaling_factor);
	gl_Position = VPMatrix * ModelMatrix * position;
	intensity = dot(normalize(sun_direction_vector), normal) ;
	intensity = max(0.4, intensity); //ambient light
	v_TexCoord = texture_uv;
	camera_dist = length((ModelMatrix * position).xyz - camera_loc);
}