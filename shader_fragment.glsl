#version 460 core

layout(location = 0) out vec4 color;

const vec4 grey = vec4(169.0 / 255.0, 169.0 / 255.0, 169.0 / 255.0, 0);

uniform sampler2D u_Texture;
uniform float atmosphere_light_damping_constant;
uniform vec4 atmosphere_light_damping_RGB_Weight;

in vec2 v_TexCoord;
in float intensity;
in float camera_dist;

void main(){
	vec4 texColor = texture(u_Texture, v_TexCoord);
	//texColor = texColor * intensity;
	vec4 lambda = exp( atmosphere_light_damping_RGB_Weight * atmosphere_light_damping_constant * camera_dist );
	texColor = texColor * lambda + (1 - lambda) * grey; 
	color = texColor;
}