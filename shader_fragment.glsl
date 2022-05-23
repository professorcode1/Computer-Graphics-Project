#version 460 core

layout(location = 0) out vec4 color;

const vec4 grey = vec4(169.0 / 255.0, 169.0 / 255.0, 169.0 / 255.0, 0);

uniform sampler2D mountain_tex;
uniform sampler2D plane_tex;

uniform float atmosphere_light_damping_constant;
uniform vec4 atmosphere_light_damping_RGB_Weight;

in vec2 v_TexCoord;
in float intensity;
in float camera_dist;
in vec4 is_mountain_tex;
void main(){
	vec4 texColor;
	if(is_mountain_tex.x < 5){
		texColor = texture(mountain_tex, v_TexCoord);
	}else{
		texColor = texture(plane_tex, v_TexCoord);
	}
	//texColor = texColor * intensity;
	vec4 lambda = exp( atmosphere_light_damping_RGB_Weight * atmosphere_light_damping_constant * camera_dist );
	texColor = texColor * lambda + (1 - lambda) * grey; 
	color = texColor;
}