#version 460 core

layout(location = 0) out vec4 color;

uniform sampler2D tree_texture;

in vec2 v_TexCoord;

void main(){
	vec4 texColor = texture(tree_texture, v_TexCoord);
	texColor.w = 1;
	color = texColor;
}