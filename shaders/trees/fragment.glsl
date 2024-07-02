#version 300 es
precision mediump float;

layout(location = 0) out vec4 color;

const vec4 fog_color = vec4(169.0 / 255.0, 169.0 / 255.0, 169.0 / 255.0, 0); //grey

uniform sampler2D tree_tex_0;

uniform float fog_density;

in vec2 v_TexCoord;
in float intensity;
in float camera_dist;


void applyExponentialFog(inout vec4 original_color, float view_distance){
	float fog_factor = pow(2.0, -1.0 * view_distance * fog_density);
	original_color = mix(fog_color, original_color, fog_factor);
}


void main(){

	vec4 texColor  = texture(tree_tex_0, v_TexCoord);
	//vec4 texColor = vec4(116.0 / 255.0,102.0 / 255.0,59.0 / 255.0,1.0);
	
	texColor = texColor * intensity;
	applyExponentialFog(texColor, camera_dist);
	color = texColor;
}