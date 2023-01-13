#version 460 core

layout(location = 0) out vec4 color;

const vec4 fog_color = vec4(169.0 / 255.0, 169.0 / 255.0, 169.0 / 255.0, 0); //grey

uniform sampler2D tree_tex_0;
uniform sampler2D tree_tex_1;
uniform sampler2D tree_tex_2;
uniform sampler2D tree_tex_3;

uniform float fog_density;
uniform int texture_to_use;

in vec2 v_TexCoord;
in float intensity;
in float camera_dist;

void applyExponentialSquareFog(inout vec4 original_color, float view_distance){
	float fog_factor = pow(2.0, -1 * view_distance * fog_density * view_distance * fog_density);
	original_color = mix(fog_color, original_color, fog_factor);
}

void applyExponentialFog(inout vec4 original_color, float view_distance){
	float fog_factor = pow(2.0, -1 * view_distance * fog_density);
	original_color = mix(fog_color, original_color, fog_factor);
}


void main(){

	vec4 texColor;
	if(texture_to_use==0){
		texColor = texture(tree_tex_0, v_TexCoord);
	}
	else if(texture_to_use==1){
		texColor = texture(tree_tex_1, v_TexCoord);
	}
	else if(texture_to_use==2){
		texColor = texture(tree_tex_2, v_TexCoord);
	}
	else if(texture_to_use==3){
		texColor = texture(tree_tex_3, v_TexCoord);
	}

	//vec4 texColor = vec4(116.0 / 255.0,102.0 / 255.0,59.0 / 255.0,1.0);
	
	texColor = texColor * intensity;
	applyExponentialFog(texColor, camera_dist);
	color = texColor;
}